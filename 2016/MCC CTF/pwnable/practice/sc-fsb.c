// gcc -m32 sc-fsb.c -z execstack -o sc-fsb
#include <stdio.h>
#include <stdlib.h>

__attribute__((constructor))
init(){
        setbuf(stdin, NULL);
        setbuf(stdout, NULL);
}

void main(void){
	int i;
	char buf[64];

	for(i=0; i<3; i++){
		printf("Input message (%d/3) >> ", i+1);
		fgets(buf, sizeof(buf), stdin);
		printf(buf);
	}
}
