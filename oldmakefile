CPROGS = $(wildcard kernel/*.c drivers/*.c drivers/vga3h/*.c drivers/serial/*.c lib/*.c cpu/*.c drivers/vga13h/*.c drivers/vgacommon/*.c fs/*.c fs/initrd/*.c)
CHEADERS = $(wildcard kernel/*.h drivers/*.h drivers/vgacommon/*.h drivers/vga13h/*.h drivers/vga3h/*.h fs/*.h fs/initrd/*.h)
OBJECTS = ${CPROGS:.c=.o}
CFLAGS = -fno-pie -m32 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -g -Icpu/ -Idrivers/ -I drivers/vgacommon/ -Ikernel/ -Ilib/ -Ifs/ -Ifs/initrd


default: kernel/kmain.elf

run: kernel/kmain.elf
	@qemu-system-x86_64 -kernel kernel/kmain.elf -append arg1 -initrd ./bootfiles/initrd 
debug: kernel/kmain.elf
	@qemu-system-x86_64 -serial file:serial.log -kernel kernel/kmain.elf  -append arg1 -initrd ./bootfiles/initrd -S -s &
	@gdb -q -x .gdbdebug

kernel/kmain.elf: ${OBJECTS} kernel/kernelheader.o cpu/int.o 
	@ld -m elf_i386 -o kernel/kmain.elf -T link.ld kernel/kernelheader.o cpu/int.o ${OBJECTS}

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
