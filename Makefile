CPROGS = $(wildcard kernel/*.c drivers/*.c drivers/vgatext/*.c drivers/serial/*.c lib/*.c cpu/*.c drivers/vga13h/*.c)
CHEADERS = $(wildcard kernel/*.h drivers/*.h)
OBJECTS = ${CPROGS:.c=.o}
#CFLAGS = -fno-pie -m32 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -g 
CFLAGS = -fno-pie -m32 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -g -Icpu/ -Idrivers/ -Ikernel/ -Ilib/



default: kernel/kmain.elf

run: kernel/kmain.elf
	@qemu-system-x86_64 -kernel kernel/kmain.elf

debug: kernel/kmain.elf
	@qemu-system-x86_64 -serial file:serial.log -kernel kernel/kmain.elf -S -s &
	@gdb -q -x gdbdebug

kernel/kmain.elf: ${OBJECTS} kernel/kernelheader.o cpu/interrupt.o
	@ld -m elf_i386 -o kernel/kmain.elf -T link.ld kernel/kernelheader.o cpu/interrupt.o ${OBJECTS}

%.o: %.c ${CHEADERS}
	@gcc ${CFLAGS} -c $< -o $@

%.o: %.asm 
	@nasm -f elf32 -o $@ $<

clean: c
c:
	@find . -name '*\.o' -exec \rm -rf {} \;
	@find . -name '*\.bin' -exec \rm -rf {} \;
	@find . -name '*\.elf' -exec \rm -rf {} \;
	@find ./kernel -name '*\.sym' -exec \rm {} \;
	@\rm serial.log 2> /dev/null
	

