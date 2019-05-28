// gcc -Wno-implicit-function-declaration -fno-stack-protector -no-pie memo.c -o memo

#include <stdio.h>
#include <stdlib.h>

__attribute__((constructor))
void init(void){
	setbuf(stdout, NULL);
}

int main(void){
	char *content;
	char buf[64];
	int size;

	do {
		printf("Input size : ");
		if(!fgets(buf, sizeof(buf), stdin))
			return -1;
		size = atoi(buf);
	} while((unsigned)size < 0x20);

	content = alloca(size);
	printf("Input Content : ");
	fgets(content, 0x20, stdin);

	printf("Your Content : %s\n", content);

	return 0;
}

__attribute__((noreturn))
void hidden(void){
	system("sh");
	exit(0);
}
