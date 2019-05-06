// gcc -m32 sc.c -o sc
#include <stdio.h>
#include <sys/mman.h>

__attribute__((constructor))
init(){
        setbuf(stdin, NULL);
        setbuf(stdout, NULL);
}

void main(void){
	void (*func)();
	
	func = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

	printf("Input your shellcode >> ");
	fgets(func, 0x1000, stdin);
	func();
}
