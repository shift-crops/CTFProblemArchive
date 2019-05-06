#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

#define TRUE		1
#define FALSE		0
#define STACK_SIZE	0x1000

void *stack_buf;
int *sp;

__attribute__((section(".stext")))
shadow_init(){
	stack_buf = mmap(NULL, STACK_SIZE, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	sp = (int*)(stack_buf + STACK_SIZE);
	asm(	"mov sp, %eax	\n"
		"mov %eax, %gs:0x20	");
}

__attribute__((section(".stext")))
shadow_fini(){
	munmap(stack_buf, STACK_SIZE);
	stack_buf = sp = NULL;
	asm(	"mov sp, %eax	\n"
		"mov %eax, %gs:0x20	");
}

__attribute__((section(".stext")))
void push(int val){
	asm(	"mov %gs:0x20, %eax	\n"
		"mov %eax, sp		");

	if((void*)sp <= stack_buf)
		_exit(1);
		
	sp--;
	mprotect(stack_buf, STACK_SIZE, PROT_WRITE);
	*sp = enc_dec(val);
	mprotect(stack_buf, STACK_SIZE, PROT_NONE);

	asm(	"mov sp, %eax		\n"
		"mov %eax, %gs:0x20	");
}

__attribute__((section(".stext")))
int pop(void){
	int val;
	asm(	"mov %gs:0x20, %eax	\n"
		"mov %eax, sp		");

	if((void*)sp >= stack_buf+STACK_SIZE)
		_exit(1);

	mprotect(stack_buf, STACK_SIZE, PROT_READ);
	val = *sp;
	mprotect(stack_buf, STACK_SIZE, PROT_NONE);
	sp++;	

	asm(	"mov sp, %eax		\n"
		"mov %eax, %gs:0x20	");

	return enc_dec(val);
}

__attribute__((section(".stext")))
int enc_dec(int addr){
	asm(	"mov 0x8(%ebp), %eax	\n"
		"xor %gs:0x18, %eax	");
}
