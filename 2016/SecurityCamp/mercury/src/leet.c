#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define BUF_SIZE 512

char buf[BUF_SIZE];

__attribute__((constructor))
void init(void){
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	system("date");
}

void leet(char*, char*);

void main(void){
	char output[BUF_SIZE];

	fprintf(stdout, "InputText : ");
	fgets(buf, sizeof(buf), stdin);
	leet(output, buf);
	fprintf(stdout, "LeetSpeak : %s\n", output);
}

void leet(char *dst, char *src){
	int i,j;
	char dict_src[]  = {'a','e','i','o','q','s','t','H','K'};
	char *dict_dst[] = {"4","3","1","0","9","5","7","|-|","|<"};

	assert(sizeof(dict_src)==sizeof(dict_dst)/sizeof(char*));

	for(i=0; src[i]^'\n'&&i<BUF_SIZE; i++){
		for(j=0; j<sizeof(dict_src); j++)
			if(src[i]==dict_src[j]){
				int sz = strlen(dict_dst[j]);
				memcpy(dst, dict_dst[j], sz);
				dst+=sz;
				goto done;
			}

		*(dst++) = src[i];
done:		continue;
	}
	*dst='\0';
}

__attribute__((section("tomori")))
void nao(void){
	asm("pop {r0, pc}");
}

