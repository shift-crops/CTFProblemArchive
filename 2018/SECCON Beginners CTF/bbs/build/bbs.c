// gcc -Wno-implicit-function-declaration -fno-stack-protector bbs.c -o bbs

#include <stdio.h>
#include <stdlib.h>

__attribute__((constructor))
void init(void){
	setbuf(stdout, NULL);
}

int main(void){
	char content[0x80];

	printf("Input Content : ");
	gets(content);

	puts("\n==============================\n");
	system("date");
	printf("%s\n\n==============================\n", content);

	return 0;
}
