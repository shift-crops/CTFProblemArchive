#include "shadow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SIZE 32

int _main(int, char**, char**);

__attribute__((constructor))
init(){
	shadow_init();
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
}

__attribute__((destructor))
fini(){
	shadow_fini();
}

void main(int argc, char *argv[], char *envp[]){
	int ret;

	ret = call(_main, argc, argv, envp);
	exit(ret);
}

int message(int, int, int);
int getnline(char*, unsigned short);

int _main(int argc, char *argv[], char *envp[]){
	char *name;

	call(printf, 	"Hello!\n"
			"You can send message three times.\n");

	name = alloca(BUF_SIZE*3/2);
	name[0]='\0';
	call(message, (int)name, BUF_SIZE/2, 3);

	call(printf, "Bye!\n");

	ret(0);
}

int message(int name, int name_len, int count){
	int i, len;

	for(i=0; i<count; i++){
		char buf[BUF_SIZE] = {0};

		if(call(strlen, name)){
			call(printf, "Change name? (y/n) : ");
			call(getnline, buf, sizeof(buf));
		}

		if(!call(strlen, name) || buf[0]=='y'){
			call(printf, "Input name : ");
			call(getnline, (char*)name ,name_len);
		}

		call(printf, "Message length : ");
		call(getnline, buf ,sizeof(buf));
		len = call(atoi, buf);
		if(len > BUF_SIZE)	len = BUF_SIZE;

		call(printf, "Input message : ");
		call(getnline, buf ,len);
		call(printf, "(%d/%d) <%s> %s\n\n", i+1, count, (char*)name, buf);
	}

	ret(0);
}

int getnline(char *buf, unsigned short len){
	int i,n;

	n = call(read, STDIN_FILENO, buf, len);
	for(i=0; i<n; i++)
		if(buf[i]=='\n'){
			buf[i]='\0';
			break;
		}

	ret(i);
}
