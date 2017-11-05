CPROGS = $(wildcard kernel/*.c drivers/*.c drivers/vgatext/*.c drivers/serial/*.c lib/*.c cpu/*.c drivers/vga13h/*.c)
CHEADERS = $(wildcard kernel/*.h drivers/*.h)
OBJECTS = ${CPROGS:.c=.o}

CFLAGS = -fno-pie -m32 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -fno-builtin  -Wall -g

qemu: osimage
	qemu-system-x86_64 -serial file:serial.log osimage.img

debug: osimage
	qemu-system-x86_64 -serial file:serial.log osimage.img -s -S &
	gdb -q -x gdbdebug

osimage: boot/bootsec.bin kernel/kmain.bin 
	@cat $^ > osimage.img

kernel/kmain.bin: ${OBJECTS}
	@nasm -f elf32 cpu/interrupt.asm
	@ld -m elf_i386 -o kernel/kmain.elf -T link.ld ${OBJECTS} cpu/interrupt.o
	@objcopy --only-keep-debug kernel/kmain.elf kernel/kmain.sym
	@objcopy -O binary kernel/kmain.elf kernel/kmain.bin

%.o: %.asm
	@nasm -f bin -o $@ $<

%.o: %.c ${CHEADERS}
	@gcc ${CFLAGS} -c $< -o $@
%.bin: %.asm
	@nasm $< -f bin -o $@

clean:
	@find . -name '*\.o' -exec \rm -rf {} \;
	@find . -name '*\.bin' -exec \rm -rf {} \;
	@find . -name '*\.elf' -exec \rm -rf {} \;
	@find ./kernel -name '*\.sym' -exec \rm {} \;
	@\rm serial.log 2> /dev/null
	@\rm osimage.img 2> /dev/null
	

