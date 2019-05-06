#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE	128
#define NOTE_SIZE	0x38
#define NOTES		0x10

static int menu(void);
static void alloc(void);
static void show(void);
static void edit(void);
static void delete(void);
static int select_id(void);

static int getnline(char *buf, int len);
static int getint(void);

char name[32];
char *note[16];

__attribute__((constructor))
int init(){
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	return 0;
}

int main(void){
	puts("==== Not So Secure Note service ====");

	printf("Tell me your name >> ");
	getnline(name, sizeof(name));

	for(;;){
		switch(menu()){
			case 1:
				alloc();
				break;
			case 2:
				show();
				break;
			case 3:
				edit();
				break;
			case 4:
				delete();
				break;
			case 0:
				goto end;
			default:
				puts("Wrong input.");
		}
	}

end:
	puts("Bye!");

	return 0;
}

int menu(void){
	printf(	"\nMENU\n"
			"1. Alloc\n"
			"2. Show\n"
			"3. Edit\n"
			"4. Delete\n"
			"0. Exit\n"
			"> ");

	return getint();
}

static void alloc(void){
	int id;

	for(id = 0; id < NOTES; id++)
		if(!note[id])
			break;

	if(id >= NOTES){
		puts("Entry is FULL...");
		return;
	}

	if(!(note[id] = (char*)calloc(NOTE_SIZE, 1))){
		puts("Allocation failed...");
		return;
	}

	printf("Input note > ");
	getnline(note[id], NOTE_SIZE);

	printf("Added id:%02d entry\n", id);
}

static void show(void){
	int id;

	if((id = select_id()) < 0)
		return;

	printf("Show id:%02d entry\n%s\n", id, note[id]);
}

static void edit(void){
	int id;

	if((id = select_id()) < 0)
		return;

	printf("Input note > ");
	getnline(note[id], NOTE_SIZE);

	printf("Edited id:%02d entry\n", id);
}

static void delete(void){
	int id;

	if((id = select_id()) < 0)
		return;

	free(note[id]);

	printf("Deleted id:%02d entry\n", id);
}

static int select_id(void){
	int id;

	printf("Input id > ");
	id = getint();

	if(id < 0 || id > NOTES){
		puts("Invalid id...");
		return -1;
	}

	if(!note[id]){
		puts("Entry does not exist...");
		return -1;
	}

	return id;
}

static int getnline(char *buf, int size){
	int len;
	char *lf;

	if(size < 0)
		return 0;

	len = read(0, buf, size-1);
	buf[len] = '\0';

	if((lf=strchr(buf,'\n')))
		*lf='\0';

	return len;
}

static int getint(void){
	char buf[BUF_SIZE];

	getnline(buf, sizeof(buf));
	return atoi(buf);
}
