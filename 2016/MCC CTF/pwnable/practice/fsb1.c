// gcc -m32 fsb1.c -o fsb1
#include <stdio.h>

int pass=0;

__attribute__((constructor))
init(){
        setbuf(stdin, NULL);
        setbuf(stdout, NULL);
}

void main(void){
	char buf[32];

	printf("Input message >> ");
	fgets(buf, sizeof(buf), stdin);
	printf(buf);

	printf("pass = 0x%08x\n", pass);
	if(pass==0xdead)
		printf("Correct!!\n");
}
