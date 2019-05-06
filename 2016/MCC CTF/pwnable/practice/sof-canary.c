// gcc -m32 sof-canary.c -fstack-protector -o sof-canary
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
	read(0, buf, 64);

	printf(	"Nice to meet you, %s\n"
		"Input message >> ", buf);
	gets(buf);
}

void get_shell(void){
        system("/bin/sh");
}

