// gcc memo.c -o memo
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>
#include <sys/mman.h>

#define BUF_SIZE 0x20

struct memo {
	char valid;
	unsigned id;
	char* desc;
};

char *key = (void*)-1;
char *logo =
	"    ____           _      _ __    __        __  ___                    \n"
	"   /  _/___ _   __(_)____(_) /_  / /__     /  |/  /__  ____ ___  ____  \n"
	"   / // __ \\ | / / / ___/ / __ \\/ / _ \\   / /|_/ / _ \\/ __ `__ \\/ __ \\ \n"
	" _/ // / / / |/ / (__  ) / /_/ / /  __/  / /  / /  __/ / / / / / /_/ / \n"
	"/___/_/ /_/|___/_/____/_/_.___/_/\\___/  /_/  /_/\\___/_/ /_/ /_/\\____/  \n"
	"\n"
	"Do you remember SecureKarte?\n"
	"He was dead, because he was not so secure.\n"
	"I am EXTREEEEEEMLY SECURE!!!!"
	"\n";

static int check_key(void);
static char *add(struct memo *list, int elms);
static void delete(struct memo *list, int elms);

static char *getnline(char *buf, int len);
static int getint(void);


__attribute__((constructor))
int init(){
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);

	return 0;
}

int main(void){
	unsigned n;
	struct memo list[3] = {};
	char *p;

	__printf_chk(1, "Key buffer size > ");
	if((unsigned)getint() > 0x40)
		key = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if(key == (void*)-1){
		key = alloca(0x40);
		memset(key, 0, 0x40);
	}

	puts(logo);

input_key:
	__printf_chk(1, "Input NEW secret key... ");
	p = getnline(key, 0x40);
	puts("OK.");

	while(n = (
		__printf_chk(1,
			"\nMENU\n"
			"1. Add\n"
			"2. Delete\n"
			"9. Change Secret Key\n"
			"0. Exit\n"
			"> ")
		, getint())){
		switch(n){
			case 1:
				if(check_key())
					add(list, sizeof(list)/sizeof(struct memo));
				break;
			case 2:
				delete(list, sizeof(list)/sizeof(struct memo));
				break;
			case 9:
				goto input_key;
			default:
				puts("Wrong input.");
				continue;
		}
		puts("Done.");
	}
	puts("Bye!");

	return 0;
}

static int check_key(void){
	char buf[0x40] = {};
	int cmp;
	static char remain = 3;

	__printf_chk(1, "Input secret key... ");
	getnline(buf, 0x40);

	if(cmp = !strncmp(key, buf, sizeof(buf))){
		puts("Key matched!");
		remain = 3;
	}
	else{
		__printf_chk(1, "Key did NOT match (remain : %d/3)\n", --remain);
		if(!remain)
			_exit(1);
	}

	return cmp;
}

static char *add(struct memo *list, int elms){
	int idx;
	char *p;
	size_t size;
	unsigned id, ofs;
	static unsigned total = 0;

	if(total > 4){
		puts("You can not add anymore!");
		return NULL;
	}

	for(idx = 0; idx < elms; idx++)
		if(!list[idx].valid)
			goto alloc;

	puts("Entry is full!!");
	return NULL;

alloc:
	__printf_chk(1, "Input size > ");
	size = getint();

	if(!(p = (char*)(size > 0x408 ? malloc(size) : calloc(1, size)))){
		puts("Alloction failed...");
		return NULL;
	}
	assert(!((unsigned long)p & 0xf));

	__printf_chk(1, "Input offset > ");
	if((ofs = getint()) >= size){
		puts("Out of bounds detected!");
		free(p);
		return NULL;
	}

	__printf_chk(1, "Input description > ");
	getnline(p + ofs, size - ofs);

	id = ++total;

	list[idx].id = id;
	list[idx].valid = 1;
	list[idx].desc = p;

	__printf_chk(1, "\nAdded id %d\n", id);

	return p + ofs;
}

static void delete(struct memo *list, int elms){
	char buf[0x10];
	unsigned id;

	__printf_chk(1, "Input id > ");
	id = getint();

	for(char i; i < elms; i++)
		if(list[i].id == id && list[i].valid){
			assert(!((unsigned long)list[i].desc & 0xf));
			list[i].valid = 0;
			free(list[i].desc);
			__printf_chk(1, "\nDeleted id %d\n", id);
			return;
		}

	puts("Entry not found...");
	return;
}

static char *getnline(char *buf, int size){
	char *lf;
	long n;

	if(!buf || size < 1)
		return NULL;

	if((n = read(0, buf, size-1)) < 0)
		return NULL;
	buf[size-1] = '\0';
	if((lf=strchr(buf, '\n')))
		*lf='\0';

	return buf+n;
}

static int getint(void){
	char buf[BUF_SIZE] = {0};

	getnline(buf, sizeof(buf));
	return atoi(buf);
}
