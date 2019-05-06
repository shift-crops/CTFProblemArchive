// gcc -m32 sof1.c -fstack-protector -o sof1
#include <stdio.h>
#include <stdlib.h>

__attribute__((constructor))
init(){
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
}

void main(void){
	char buf1[32], buf2[32]="hogefuga";

	printf("Input Name >> ");
	gets(buf1);
	printf("buf1 : %s\nbuf2 : %s\n", buf1, buf2);

	if(!strcmp(buf2,"tuat"))
		printf("Correct!\n");
}
