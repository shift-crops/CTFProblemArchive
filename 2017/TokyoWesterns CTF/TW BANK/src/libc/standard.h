#ifndef _STANDARD_H
#define _STANDARD_H

#include "common.h"

int read(int fd, void *buf, unsigned int count);
int write(int fd, void *buf, unsigned int count);
int open(char *pathname, int flags, int mode);
int close(int fd);
void exit(int code);
void *mmap(void *addr, unsigned int length, int prot, int flag, int fd, unsigned int offset);
int munmap(void *addr, unsigned int length);

int printf(char *fmt, ...);
int sprintf(char *buf, char *fmt, ...);
int puts(char *buf);
int putchar(int c);

#endif
