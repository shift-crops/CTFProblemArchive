compile: shadow.c func.obj
	gcc -m32 -Wl,-z,relro,-z,now shadow.c func.obj -o shadow	# enable Full RELRO
	execstack -c shadow						# enable NX bit

full:
	make clean
	make compile

clean:
	rm -f *.o *.obj *.bin

%.obj: %.asm
	nasm -f elf -o $*.obj $*.asm
