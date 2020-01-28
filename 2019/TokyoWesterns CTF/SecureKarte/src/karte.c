// gcc karte.c -Wl,-z,relro,-z,lazy -fno-PIE -no-pie -o karte
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 32

char reserved[0x80] = "reserved";
int zfd = -1, rfd = -1;

__attribute__ ((section (".karte")))
struct karte {
	char valid;
	unsigned id;
	char* desc;
} list[3];

__attribute__ ((section (".karte")))
unsigned long lock = -1;

unsigned long key = 0;
char name[0x40];

static long getrand(void);
static int menu(void);

static void add(void);
static void delete(void);
static void modify(void);

static int getnline(char *buf, int len);
static int getint(void);


__attribute__((constructor))
int init(){
	setbuf(stdout, NULL);

	zfd = open("/dev/zero", O_RDONLY);
	rfd = open("/dev/urandom", O_RDONLY);

	lock = key = getrand();

	return 0;
}

__attribute__((destructor))
int fini(){
	if(zfd != -1)
		close(zfd);
	if(rfd != -1)
		close(rfd);
}

int main(void){
	int n;

	if(zfd == -1 || rfd == -1)
		return -1;

	printf(
		" _______  _______  _______  __   __  ______    _______    ___   _  _______  ___      _______  _______ \n"
		"|       ||       ||       ||  | |  ||    _ |  |       |  |   | | ||   _   ||   |    |       ||       |\n"
		"|  _____||    ___||       ||  | |  ||   | ||  |    ___|  |   |_| ||  |_|  ||   |    |_     _||    ___|\n"
		"| |_____ |   |___ |       ||  |_|  ||   |_||_ |   |___   |      _||       ||   |      |   |  |   |___ \n"
		"|_____  ||    ___||      _||       ||    __  ||    ___|  |     |_ |       ||   |___   |   |  |    ___|\n"
		" _____| ||   |___ |     |_ |       ||   |  | ||   |___   |    _  ||   _   ||       |  |   |  |   |___ \n"
		"|_______||_______||_______||_______||___|  |_||_______|  |___| |_||__| |__||_______|  |___|  |_______|\n"
		"\n\n"  );
input_name:
	printf("Input patient name... ");
	getnline(name, sizeof(name));
	puts("OK.");

	while(n = menu()){
		switch(n){
			case 1:
				add();
				break;
			case 2:
				printf(	"No no no...\n"
						"This is SECURE kalte.\n"
						"No one can see entries. ( ´,_ゝ`)\n\n");
				break;
			case 3:
				delete();
				break;
			case 4:
				modify();
				break;
			case 99:
				goto input_name;
			default:
				puts("Wrong input.");
				continue;
		}
		puts("Done.");
	}
	puts("Bye!");

	return 0;
}

static int menu(void){
	printf(	"\nMENU (patient : CENSORED)\n"
			"!#!#!#!#!#!#!#!#!#\n"
			"1.  Add\n"
			"2.  Show\n"
			"3.  Delete\n"
			"4.  Modify\n"
			"99. Rename patient\n"
			"0.  Exit\n"
			"!#!#!#!#!#!#!#!#!#\n"
			"> ");

	return getint();
}

static long getrand(void){
	long value;

	read(rfd, &value, sizeof(value));
	return value;
}

static void add(void){
	int idx;
	char *p;
	unsigned id;
	size_t size;

	for(idx = 0; idx < sizeof(list)/sizeof(struct karte); idx++)
		if(!list[idx].valid)
			goto alloc;

	puts("karte is full!!");
	return;

alloc:
	printf("Input size > ");
	size = getint();

	if(!(p = (char*)(size > 0x800 ? malloc(size) : calloc(1, size)))){
		puts("alloction failed...");
		return;
	}

	if(size > 0x800)
		read(zfd, p, size);

	printf("Input description > ");
	getnline(p, size);

	id = getrand() & 0xffff;
	for(int i = 0; i < sizeof(list)/sizeof(struct karte); i++)
		if(list[i].id == id && list[i].valid){
			id = getrand() & 0xffff;
			i = 0;
		}

	list[idx].id = id;
	list[idx].valid = 1;
	list[idx].desc = p;

	printf("\nAdded id %d\n", id);
}

static void delete(void){
	unsigned id;

	printf("Input id > ");
	id = getint();

	for(int i = 0; i < sizeof(list)/sizeof(struct karte); i++)
		if(list[i].id == id && list[i].valid){
			list[i].valid = 0;
			free(list[i].desc);
			printf("\nDeleted id %d\n", id);
			return;
		}

	puts("karte not found...");
	return;
}

static void modify(void){
	unsigned id;

	if(key != lock){
		puts("Hey! You can't modify karte any more!!");
		return;
	}

	printf("Input id > ");
	id = getint();

	for(int i = 0; i < sizeof(list)/sizeof(struct karte); i++)
		if(list[i].id == id){
			key = 0xdeadc0bebeef;
			printf("Input new description > ");
			getnline(list[i].desc, strlen(list[i].desc)+1);
			printf("\nModified id %d\n", id);
			return;
		}

	puts("karte not found...");
	return;
}

static int getnline(char *buf, int size){
	char *lf;
	long n;

	if(!buf || size < 1)
		return 0;

	if((n = read(0, buf, size-1)) < 0)
		return -1;
	buf[size-1] = '\0';
	if((lf=strchr(buf, '\n')))
		*lf='\0';

	return 1;
}

static int getint(void){
	char buf[BUF_SIZE] = {0};

	getnline(buf, sizeof(buf));
	return atoi(buf);
}
