#ifndef _UTILS_H
#define _UTILS_H

#include "common.h"

int strlen(char *s);
char *strcat(char *d, char *s);
char *strncat(char *d, char *s, unsigned int n);
char *strchr(char *s, int c);
char *strdup(char *s);
int atoi(char *s);
char *itoa(int v, char *s);

#endif
