// gcc -m32 sof-plt.c -fno-stack-protector -o sof-plt
#include <stdio.h>
#include <stdlib.h>

__attribute__((constructor))
init(){
        setbuf(stdin, NULL);
        setbuf(stdout, NULL);
}

void main(void){
	char buf[32];

        system("echo 'Hello'");

	printf("Input Name >> ");
	gets(buf);
}
