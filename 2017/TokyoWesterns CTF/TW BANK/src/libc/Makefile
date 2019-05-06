TARGET  = libtinyc.so
OBJS    = libc.o utils.o heap.o funcs.o

CC      := gcc
AS      := nasm
CFLAGS  += -Wall -masm=intel -nostdlib -m32 -fPIE
LDFLAGS += -shared

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)
	execstack -c $(TARGET)
	strip $(TARGET)

all :
	make clean
	make $(TARGET)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.o : %.asm
	$(AS) -f elf $<

clean:
	$(RM) $(OBJS) $(TARGET)

