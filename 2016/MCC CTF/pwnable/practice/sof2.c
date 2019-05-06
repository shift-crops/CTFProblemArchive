// gcc -m32 sof2.c -fno-stack-protector -o sof2
#include <stdio.h>
#include <stdlib.h>

__attribute__((constructor))
init(){
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
}

void main(void){
	char buf[32];

	printf("Input Name >> ");
	gets(buf);
}

void get_shell(void){
        system("/bin/sh");
}

