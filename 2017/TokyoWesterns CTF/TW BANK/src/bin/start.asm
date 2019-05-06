global _start
extern libc_start_main, main

_start:
	push esp
	push main
	call libc_start_main
	hlt
