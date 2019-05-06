#include "utils.h"
#include "heap.h"

int strlen(char *s){
        int i;
        for(i=0; s[i]; i++);
        return i;
}

char *strcat(char *d, char *s){
        char *p;

        for(p=d; *p; p++);
        for(; *s; p++, s++)
		*p = *s;
	*p = '\0';

        return d;
}

char *strncat(char *d, char *s, unsigned int n){
        char *p;
	unsigned int i;

        for(p=d; *p; p++);
        for(i=0; i<n && *s; i++, s++)
		p[i] = *s;
	p[i] = '\0';

        return d;
}

char *strchr(char *s, int c){
	for(; *s && (*s ^ (char)c); s++);
	return *s ? s : NULL;
}

char *strdup(char *s){
	int i;
        char *p;

	p = malloc(strlen(s)+1);
        for(i=0; *s; i++, s++)
		p[i] = *s;
	p[i] = '\0';

        return p;
}

int atoi(char *s){
	int i, v = 0, sig = 1;

	for(i=0; !(s[i]^' '); i++);
	if(s[i]=='-'){
		sig = -1;
		i++;
	}

	for(; s[i]>='0' && s[i]<='9'; i++){
		v *= 10;
		v += s[i]-'0';
	}

	return v*sig;
}

char *itoa(int v, char *s){
	int i, n, _v;

	if(v<0){
		v *= -1;
		*(s++) = '-';
	}

	for(n=0, _v=v; _v/=10; n++);
	for(i=n; i>=0; i--, v/=10)
		s[i] = '0' + v%10;
	s[n+1] = '\0';

	return s;
}
