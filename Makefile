C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
CFLAGS=-ffreestanding -nostdlib -fno-stack-protector 

OBJ = ${C_SOURCES:.c=.o}

all: kernel.bin bootsec.bin os-img

kernel.bin: ${OBJ}
	ld -o $@ -Ttext 0x1000 $^ --oformat binary

%.o : %.c
	gcc $(CFLAGS) -c $< -o $@

bootsec.bin:	
	@nasm ./boot/bootsec.asm -f bin -o bootsec.bin

os-img: kernel.bin bootsec.bin
	@cat bootsec.bin kernel.bin > myos.img

qemu: os-img
	@qemu-system-x86_64 myos.img

clean:
	\rm -rf *.o *.bin *.img
	find . -name '*\.o' -exec rm -rf {} \;
