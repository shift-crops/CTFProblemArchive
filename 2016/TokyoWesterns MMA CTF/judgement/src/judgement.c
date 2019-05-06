#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define BUF_SIZE 64

char flag[BUF_SIZE];

int load_flag(char*, char*, int);
int getnline(char*, int);

__attribute__((constructor))
init(){
	char flag_name[]="flag.txt";

	setbuf(stdin,NULL);
	setbuf(stdout,NULL);
	if(!load_flag(flag_name, flag, sizeof(flag))){
		printf("Loading '%s' failed...\n", flag_name);
		_exit(0);
	}
}

int main(void){
	char buf[BUF_SIZE];

	alloca(0x80);
	printf(	"Flag judgment system\n"
		"Input flag >> ");
	if(!getnline(buf, sizeof(buf))){
		puts("Unprintable character");
		return -1;
	}
	printf(buf);

	if(strcmp(buf, flag))
		puts("\nWrong flag...");
	else
		puts("\nCorrect flag!!");

}

int load_flag(char *fname, char *buf, int len){
	FILE *fp;
	char *lf;

	if(!(fp = fopen(fname, "r")))
		return 0;

	if(!fgets(buf, len, fp))
		return 0;

	if(lf=strchr(buf, '\n'))
		*lf = '\0';
	return 1;
}

int getnline(char *buf, int len){
	char *lf;
	int i;

	fgets(buf, len, stdin);
	if(lf=strchr(buf, '\n'))
		*lf = '\0';
	len = strlen(buf);

	for(i=0; i<len; i++)
		if(!isprint(buf[i]))
			break;
	return i==len;
}
