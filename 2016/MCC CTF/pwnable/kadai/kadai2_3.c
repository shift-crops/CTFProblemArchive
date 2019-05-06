// gcc -m32 kadai2_3.c -z execstack -o kadai2
// gcc -m32 kadai2_3.c -o kadai3
#include <stdio.h>
#include <stdlib.h>
#define BUF_SIZE 32

__attribute__((constructor))
init(){
        setbuf(stdin, NULL);
        setbuf(stdout, NULL);
}

void fgets_wrapper(char*, unsigned short);

void main(void){
	char buf[BUF_SIZE], format[BUF_SIZE]="Your message : %s\n";
	int len;

	printf("Message length >> ");
	fgets_wrapper(buf, sizeof(buf));
	len = atoi(buf);

	if(len > (int)sizeof(buf)){
		printf("length should be less than %d\n", sizeof(buf));
		len = sizeof(buf);
	}

	printf("Input message(%d) >> ", len);
	fgets_wrapper(buf, len);

	printf(format, buf);

	exit(0);
}

void fgets_wrapper(char *buf, unsigned short len){
	fgets(buf, len, stdin);
}
