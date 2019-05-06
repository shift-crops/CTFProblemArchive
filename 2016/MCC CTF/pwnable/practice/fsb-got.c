// gcc -m32 fsb-got.c -o fsb-got
#include <stdio.h>
#include <stdlib.h>

__attribute__((constructor))
init(){
        setbuf(stdin, NULL);
        setbuf(stdout, NULL);
}

void main(void){
	char buf[64];

	printf("Input message >> ");
	fgets(buf, sizeof(buf), stdin);
	printf(buf);

	exit(0);
}

void get_shell(void){
        system("/bin/sh");
}
