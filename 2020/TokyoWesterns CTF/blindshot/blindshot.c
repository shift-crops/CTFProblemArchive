// gcc blindshot.c -o blindshot

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int service(char fd);

__attribute__((constructor))
int init(void){
	setbuf(stdin, NULL);
	sleep(1);
	alarm(5);
}

int main(void){
	int nullfd;

	if((nullfd = open("/dev/null", O_WRONLY)) < 0)
		return -1;

	return service(nullfd);
}

int service(char fd){
	char *buf;
	int n;

	dprintf(1, "> ");

	if(!scanf("%200ms", &buf))
		_exit(-1);

	if((n = dprintf(fd, buf)) < 0)
		_exit(-1);

	dprintf(1, "done.\n");

	return n;
}
