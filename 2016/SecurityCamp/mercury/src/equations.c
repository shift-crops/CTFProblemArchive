// gcc -m32 equations.c -o equations
// ./equations 9 -7 2
#include <stdio.h>

int check(int, int, int);
int calc_flag(int, int, int);

int main(int argc, char *argv[]){
	int a,b,c;

	if(argc<4){
		printf("Usage %s <a> <b> <c>\n", argv[0]);
		return -1;
	}

	a = atoi(argv[1]);
	b = atoi(argv[2]);
	c = atoi(argv[3]);

	if(check(a, b, c))
		printf("Congratz! flag is FLAG{%d}\n", calc_flag(a,b,c));
	else
		printf("Wrong key...\n");

	return 0;
}

int check(int a, int b, int c){
	if(a*3+b*5+c*4)	return 0;
	if(a*2+b*4+c*5)	return 0;
	if(a-b-c*8)	return 0;

	return 1;
}

int calc_flag(int a, int b, int c){
	int i,v=1;

	for(i=1; i<=a; i++)
		v*=i;
	for(i=1; i<=b; i++)
		v-=i;

	return v<<c;
}
