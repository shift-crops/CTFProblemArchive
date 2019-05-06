#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define BUF_SIZE 64
#define NOTES 16

int menu(void);
void add_note(void);
void show_note(void);
void remove_note(void);
int getnline(char *buf, int size);
int getint(void);

char *note_list[NOTES];

__attribute__((constructor))
void init(void){
	setbuf(stdout, NULL);
}

int main(void){
	int n;
	puts(	"SECCON Beginners NEXT course\n"
		"Pwnable Problem");

	while(n = menu()){
		puts("");

		switch(n){
			case 0:
				goto end;
			case 1:
				add_note();
				break;
			case 2:
				show_note();
				break;
			case 3:
				remove_note();
				break;
			default:
				puts("Invalid input...");
		}
		puts("done.");
	}

end:
	return 0;
}

int menu(void){
	printf( "\n"
		"1. add\n"
		"2. show\n"
		"3. remove\n"
		"0. exit\n"
		">> ");

	return getint();
}

void add_note(void){
	int i, size;
	char *buf;

	for(i=0; i<NOTES && note_list[i]; i++);

	if(i>=NOTES){
		puts("can't add note any more...");
		return;
	}

	puts("NEW NOTES");
	printf("Input note length...");
	size = getint();
	if(!(buf = malloc(size))){
		puts("can not allocate...");
		return;
	}
	
	printf("Input note...");
	getnline(buf, size);
	note_list[i] = buf;
}

void show_note(void){
	int i;

	puts("SHOW NOTES");
	for(i=0; i<NOTES; i++)
		if(note_list[i])
			printf("%02d : %s\n", i, note_list[i]);
}

void remove_note(void){
	int id;

	puts("DELETE NOTES");

	printf("Input id to remove...");
	id = getint();

	if(id < 0 || id > NOTES-1)
		puts("out of length");
	else if(!note_list[id])
		puts("not exits...");
	else
		free(note_list[id]);
}

int getnline(char *buf, int size){
	char *lf;
	int n;

	if(size < 1 || !(n = read(STDIN_FILENO, buf, size-1)))
		return 0;

	buf[n] = '\0';
	if((lf=strchr(buf,'\n')))
		*lf='\0';

	return n;
}

int getint(void){
	char buf[BUF_SIZE]={0};

	getnline(buf, sizeof(buf));
	return atoi(buf);
}

