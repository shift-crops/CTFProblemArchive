// gcc -Wl,-z,relro,-z,now -fno-stack-protector load.c -o load && strip load

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE 32

void initialize(void);
void finalize(void);
void load(char *buf, char *fname, off_t offset, size_t size);
void getnline(char *buf, int size);
int getint(void);

char fname[0x80];

int main(void){
	char buf[BUF_SIZE];
	off_t offset;
	size_t size;

	initialize();

	__printf_chk(1, "Load file Service\n"
		"Input file name: ");
	getnline(fname, sizeof(fname));

	__printf_chk(1, "Input offset: ");
	offset = getint();
	__printf_chk(1, "Input size: ");
	size = getint();

	load(buf, fname, offset, size);

	finalize();

	return 0;
}

void initialize(void){
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
}

void finalize(void){
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

void load(char *buf, char *fname, off_t offset, size_t size){
	int fd;

	if((fd = open(fname, O_RDONLY)) == -1){
		puts("You can't read this file...");
		return;
	}

	lseek(fd, offset, SEEK_SET);
	if(read(fd, buf, size) > 0)
		puts("Load file complete!");

	close(fd);
}

void getnline(char *buf, int size){
	char *lf;

	if(size < 0)
		return;

	fgets(buf, size, stdin);
	if((lf=strchr(buf,'\n')))
		*lf='\0';
}

int getint(void){
	char buf[BUF_SIZE];

	buf[0] = 0x00;
	getnline(buf, sizeof(buf));
	return atoi(buf);
}
