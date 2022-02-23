// gcc smash.c -fno-stack-protector -o smash && strip smash
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

static void func(void);
static char *getnline(char *p, size_t size);

int main(void){
	func();
	_exit(0);
}

static void func(void){
	char *p;

	dprintf(1, "Input name > ");
	p = getnline(NULL, 0x20);
	__dprintf_chk(1, 1, p);
	dprintf(1, "\nOK? [y/n] ");

	p = getnline(NULL, 0x38);
	if((p[0]|0x20) == 'y'){
		dprintf(1, "\nInput message > ");
		getnline(p, 0x38);
	}

	dprintf(1, "\nBye!\n");
}

static char *getnline(char *p, size_t size){
	char buf[0x20];
	int n;

	if(size < 1 || (n = read(0, buf, size-1)) < 0)
		return 0;
	buf[n] = '\0';

	return p ? strncpy(p, buf, malloc_usable_size(p)) : strdup(buf);
}
