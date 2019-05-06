// gcc -m32 fsb2.c -o fsb2
#include <stdio.h>
#include <stdlib.h>

__attribute__((constructor))
init(){
        setbuf(stdin, NULL);
        setbuf(stdout, NULL);
}

void main(void){
	char buf[64];

	printf("Input message (%p) >> ", buf);
	fgets(buf, sizeof(buf), stdin);
	printf(buf);
}

void get_shell(void){
	system("/bin/sh");
}
