#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 32

int getnline(char *buf, int size);
long getlong(void);

int main(void){
	char buf[0x10];
	long *p;

	dprintf(STDOUT_FILENO, "Do you want to rewite memory?\n>> ");
	getnline(buf, 0x60);

	if(strcasecmp(buf, "yes"))
		return 0;

	dprintf(STDOUT_FILENO, "addr : ");
	p = (long*)getlong();
	dprintf(STDOUT_FILENO, "value : ");
	*p = getlong();
}

int getnline(char *buf, int size){
	char *lf;
	int n;

	if(size < 1 || !(n = read(STDIN_FILENO, buf, size-1)))
		return 0;

	buf[n] = '\0';
	if((lf=strchr(buf,'\n')))
		*lf='\0';

	return n;
}

long getlong(void){
	char buf[BUF_SIZE]={0};

	getnline(buf, sizeof(buf));
	return atol(buf);
}
