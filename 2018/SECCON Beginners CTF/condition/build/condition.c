// gcc -Wno-implicit-function-declaration -fno-stack-protector condition.c -o condition

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void read_file(char *fname);

__attribute__((constructor))
void init(void){
	setbuf(stdout, NULL);
}

int main(void){
	char name[0x20];
	unsigned int cond = 0;

	printf("Please tell me your name...");
	gets(name);

	if(cond == 0xdeadbeef){
		puts("OK! You have permission to get flag!!");
		read_file("flag.txt");
	}
	else
		puts("Permission denied");

	exit(0);
}

void read_file(char *fname){
	char buf[128];
	FILE *fp;
	size_t len;

	fp = fopen(fname, "r");
	while((len = fread(buf, 1, sizeof(buf), fp)) > 0)
		write(STDOUT_FILENO, buf, len);
}
