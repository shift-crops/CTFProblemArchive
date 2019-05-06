#include "standard.h"
#include "utils.h"
#include "heap.h"

int _libc_args(void *esp, char ***argv, char ***envp);
int _read(int fd, void *buf, unsigned int count);
int _write(int fd, void *buf, unsigned int count);
int _open(char *pathname, int flags, int mode);
int _close(int fd);
void _exit(int code);
int _munmap(void *addr, unsigned int length);
void *_mmap2(void *addr, unsigned int length, int prot, int flag, int fd, unsigned int offset);

void libc_start_main(void (*func)(), void *esp){
	int argc;
	char **argv, **envp;

	argc = _libc_args(esp, &argv, &envp);

	func(argc, argv, envp);
	exit(0);
}

/* syscall */
int read(int fd, void *buf, unsigned int count){
	return count > 0 ? _read(fd, buf, count) : 0;
}

int write(int fd, void *buf, unsigned int count){
	return count > 0 ? _write(fd, buf, count) : 0;
}

int open(char *pathname, int flags, int mode){
	return _open(pathname, flags, mode);
}

int close(int fd){
	return _close(fd);
}

__attribute__((noreturn))
void exit(int code){
	_exit(code);
}

int munmap(void *addr, unsigned int length){
	return _munmap(addr, length);
}

void *mmap(void *addr, unsigned int length, int prot, int flag, int fd, unsigned int offset){
	return _mmap2(addr, length, prot, flag, fd, offset);
}

/* I/O */
int printf(char *fmt, ...){
	int n, cnt;
	char *p;

	n = strlen(fmt);
	for(p=fmt, cnt=3; (p = strchr(p, '%')); p+=2){
		register int arg;

		asm("mov %0, [ebp+%1*0x4]" : "=r"(arg) : "r"(cnt));
		switch(*(p+1)){
			case 'd':
				n += 10;
				break;
			case 's':
				n += strlen((char*)arg);
				break;
		}
		n -= 2;
		cnt++;
	}

	if(cnt > 3){
		char *buf = malloc(n+1);
		if(!buf)
			return -1;

		buf[0] = '\0';

		for(cnt=3; (p = strchr(fmt, '%'));){
			register int arg;

			strncat(buf, fmt, p-fmt);
			asm("mov %0, [ebp+%1*0x4]" : "=r"(arg) : "r"(cnt));
			switch(*(p+1)){
				case 'd':
					itoa(arg, buf + strlen(buf));
					break;
				case 's':
					strncat(buf, (char*)arg, n+1 - strlen(buf));
					break;
			}
			cnt++;
			fmt = p+2;
		}
		strncat(buf, fmt, n+1 - strlen(buf));
		write(1, buf, strlen(buf));
		free(buf);
	}
	else
		write(1, fmt, n);
/*
	for(p=fmt, cnt=3; (p = strchr(p, '%')); p+=2){
		register int arg;

		n += write(1, fmt, p-fmt);
		asm("mov %0, [ebp+%1*0x4]" : "=r"(arg) : "r"(cnt));
		switch(*(p+1)){
			case 'd':
				itoa(arg, buf);
				n += write(1, buf, strlen(buf));
				cnt++;
				break;
			case 's':
				n += 
				write(1, (char*)arg, strlen((char*)arg));
				cnt++;
				break;
			default:
				n += write(1, p, 2);
		}
		fmt = p+2;
	}
	n += write(1, fmt, strlen(fmt));
*/
	return n;
}

int sprintf(char *buf, char *fmt, ...){
	int cnt;
	char *p;

	buf[0] = '\0';
	for(cnt=4; (p = strchr(fmt, '%'));){
		register int arg;

		strncat(buf, fmt, p-fmt);
		asm("mov %0, [ebp+%1*0x4]" : "=r"(arg) : "r"(cnt));
		switch(*(p+1)){
			case 'd':
				itoa(arg, buf + strlen(buf));
				break;
			case 's':
				strcat(buf, (char*)arg);
				break;
		}
		cnt++;
		fmt = p+2;
	}
	strcat(buf, fmt);

	return strlen(buf);
}

int puts(char *buf){
	int n;

	if((n = write(1, buf, strlen(buf))))
		write(1, "\n", 1);

	return n;
}

int putchar(int c){
	write(1, (char*)&c, 1);
	return c;
}
