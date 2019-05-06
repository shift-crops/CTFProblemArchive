TARGET  = twbank
OBJS    = start.o bank.o

CC      := gcc
AS      := nasm
CFLAGS  += -Wall -I../libc -masm=intel -nostdlib -m32
LDFLAGS += -Wl,-z,now,-z,relro -L../libc -ltinyc
#LDFLAGS += -L./libc -ltinyc

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)
	execstack -c $(TARGET)

all :
	make clean
	make $(TARGET)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.o : %.asm
	$(AS) -f elf $<

clean:
	$(RM) $(OBJS) $(TARGET)

