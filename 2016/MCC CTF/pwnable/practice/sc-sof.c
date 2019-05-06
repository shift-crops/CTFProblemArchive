// gcc -m32 sc-sof.c -z execstack -fno-stack-protector -o sc-sof
#include <stdio.h>

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
