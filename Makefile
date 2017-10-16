CPROGS = $(wildcard kernel/*.c drivers/*.c lib/*.c)
CHEADERS = $(wildcard kernel/*.h drivers/*.h)
OBJECTS = ${CPROGS:.c=.o}

CFLAGS = -fno-pie -m32 -ffreestanding -fno-stack-protector -nostdlib -Wall

osimage: boot/bootsec.bin kernel/kmain.bin
	cat $^ > osimage.img

kernel/kmain.bin: ${OBJECTS}
	ld -m elf_i386 -o kernel/kmain.bin -T link.ld --oformat binary ${OBJECTS}

%.o: %.asm
	nasm -f bin -o $@ $<

%.o: %.c ${CHEADERS}
	gcc ${CFLAGS} -c $< -o $@
%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	find . -name '*\.o' -exec \rm -rf {} \;
	find . -name '*\.bin' -exec \rm -rf {} \;
	\rm osimage.img

