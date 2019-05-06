compile:
	make sof
	make fsb
	make sc

clean:
	rm *.bin

all:
	make clean
	make compile

sof: sof1.c sof2.c sof-canary.c sof-plt.c
	gcc -m32 sof1.c -fstack-protector -o sof1.bin
	gcc -m32 sof2.c -fno-stack-protector -o sof2.bin
	gcc -m32 sof-canary.c -fstack-protector -o sof-canary.bin
	gcc -m32 sof-plt.c -fno-stack-protector -o sof-plt.bin

fsb: fsb1.c fsb2.c fsb3.c fsb-got.c fsb-libc.c
	gcc -m32 fsb1.c -o fsb1.bin
	gcc -m32 fsb2.c -o fsb2.bin
	gcc -m32 fsb3.c -o fsb3.bin
	gcc -m32 fsb-got.c -o fsb-got.bin
	gcc -m32 fsb-libc.c -o fsb-libc.bin

sc: sc.c sc-sof.c sc-fsb.c
	gcc -m32 sc.c -o sc.bin
	gcc -m32 sc-sof.c -z execstack -fno-stack-protector -o sc-sof.bin
	gcc -m32 sc-fsb.c -z execstack -o sc-fsb.bin
