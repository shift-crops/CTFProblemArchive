// gcc babyheap.c -o babyheap
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 0x30

static int menu(void);
static int getnline(char *buf, int size);
static int getint(void);

__attribute__((constructor))
int init(){
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	return 0;
}

int main(void){
	int n;
	char *content;
	
	printf( "Welcome to babyheap challenge!\n"
			"Present for you!!\n"
			">>>>> %p <<<<<\n", stdin);

	while(n = menu()){
		switch(n){
			case 1:
				if(content)
					puts("No Space!!");
				else{
					content = malloc(BUF_SIZE);
					printf("Input Content: ");
					getnline(content, BUF_SIZE);
				}
				break;
			case 2:
				free(content);
				break;
			case 3:
				content = NULL;
				break;
		}
	}
	return 0;
}

static int menu(void){
	printf(	"\nMENU\n"
			"1. Alloc\n"
			"2. Delete\n"
			"3. Wipe\n"
			"0. Exit\n"
			"> ");

	return getint();
}

static int getnline(char *buf, int size){
	int len;
	char *lf;

	if(size < 0)
		return 0;

	len = read(0, buf, size-1);
	buf[len] = '\0';

	if((lf=strchr(buf,'\n')))
		*lf='\0';

	return len;
}

static int getint(void){
	char buf[BUF_SIZE];

	getnline(buf, sizeof(buf));
	return atoi(buf);
}
