// gcc onlyonce.c -o onlyonce
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 128

struct Entry {
	char *memo;
	int inuse;
};

char name[0x10] = {};
int oo = 0;
struct Entry entry[8] = {};

__attribute__((constructor))
void init(void){
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	alarm(60);
	srand(0);
}

static int menu(void);
static void check_freespace(void);
static void new_memo(void);
static void show_memo(void);
static void remove_memo(void);
static void only_once(void);

static int getnline(char *buf, int size);
static int getint(void);

int main(void){
	int choice;

	printf(	"==== Only Once Memo ===\n"
		"Please tell me owner's name... ");
	getnline(name, sizeof(name));

	while(choice = menu()){
		check_freespace();

		switch(choice){
			case 1:
				new_memo();
				break;
			case 2:
				show_memo();
				break;
			case 3:
				remove_memo();
				break;
			case 4:
				only_once();
				break;
			default:
				printf(	"Wrong input...\n");
		}
		printf("Done\n\n");
	}

	printf(	"Bye!\n");
}

static int menu(void){
	printf(	"1. New memo\n"
		"2. Show memo\n"
		"3. Remove memo\n"
		"0. Discard all memos\n"
		">> ");

	return getint();
}

static void check_freespace(void){
	unsigned int idx;

	for(idx = 0; idx < sizeof(entry)/sizeof(struct Entry); idx++)
		if(!entry[idx].inuse)
			break;

	if(idx < sizeof(entry)/sizeof(struct Entry))
		return;

	printf(	"memo is full!\n"
		"OOM-Killer(?) made one free space!\n");
	idx = rand() % (sizeof(entry)/sizeof(struct Entry));
	free(entry[idx].memo);
	entry[idx].inuse = 0;
}

static void new_memo(void){
	unsigned int idx;
	char buf[BUF_SIZE]={0};

	for(idx = 0; idx < sizeof(entry)/sizeof(struct Entry); idx++)
		if(!entry[idx].inuse)
			break;

	if(idx >= sizeof(entry)/sizeof(struct Entry)){
		printf("There is no space to write new memo.\n");
		return;
	}

	printf(	"Please input your new memo.\n"
		">> ");
	getnline(buf, sizeof(buf));
	entry[idx].memo = strdup(buf);
	entry[idx].inuse = 1;
}

static void show_memo(void){
	unsigned int idx;

	for(idx = 0; idx < sizeof(entry)/sizeof(struct Entry); idx++)
		if(entry[idx].inuse)
			printf("%d : %s\n", idx, entry[idx].memo);
}

static void remove_memo(void){
	unsigned int idx;

	printf(	"Which memo do you want to remove.\n"
		">> ");
	idx = getint();

	if(idx >= sizeof(entry)/sizeof(struct Entry) || !entry[idx].memo){
		printf("There is no memo.\n");
		return;
	}

	free(entry[idx].memo);
	entry[idx].memo = NULL;
	entry[idx].inuse = 0;
}

static void only_once(void){
	unsigned int idx;

	if(oo > 0){
		printf(	"Hey! You have already called me!\n");
		exit(0);
	}
	oo++;

	printf(	"Which memo do you want to rewrite.\n"
		">> ");
	idx = getint();

	if(idx >= sizeof(entry)/sizeof(struct Entry) || !entry[idx].inuse){
		printf("There is no memo.\n");
		return;
	}

	printf(	"Please input your new memo.\n"
		">> ");
	getnline(entry[idx].memo, strlen(entry[idx].memo)+1);
}

static int getnline(char *buf, int size){
	char *lf;
	int n;

	if(size < 1 || !(n = read(STDIN_FILENO, buf, size-1)))
		return 0;

	buf[n] = '\0';
	if((lf=strchr(buf,'\n')))
		*lf='\0';

	return n;
}

static int getint(void){
	char buf[BUF_SIZE]={0};

	getnline(buf, sizeof(buf));
	return atoi(buf);
}
