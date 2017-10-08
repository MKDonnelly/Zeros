#!/bin/bash

CFLAGS="-fno-pie -m32 -ffreestanding -fno-stack-protector -nostdlib -Wall"
CPROGS="kmain.c libprint.c libvgaentry.c libportio.c libcursor.c libscroll.c libscreencap.c libtiming.c libcpu.c libkeyboard.c"
OBJFILES="kmain.o libprint.o libvgaentry.o libportio.o libcursor.o libscroll.o libscreencap.o libtiming.o libcpu.o libkeyboard.o"


if [[ "$1" == "clean" ]]
then
   \rm -rf *.o *.bin *.img
   exit
fi

#Compile assembly programs
nasm -f bin -o bootsec.bin bootsec.asm 

#The kernel and libraries
for cprog in $CPROGS
do
   gcc $CFLAGS -c $cprog -o ${cprog%.c}.o
done

nasm -f elf32 -o idt.o idt.asm

ld -m elf_i386 -o kmain.bin -T link.ld --oformat binary $OBJFILES idt.o

#Combine the bootsector and kernel into a single image
cat bootsec.bin kmain.bin > osimage.img

exit

#compile kernel libraries
gcc $CFLAGS -c libprint.c -o libprint.o

#Compile c kernel
#gcc -fno-pie -m32 -ffreestanding -c kmain.c -o kmain.o
gcc $CFLAGS -c kmain.c -o kmain.o
#ld -m elf_i386 -o kmain.bin -T link.ld kmain.o --oformat binary
ld -m elf_i386 -o kmain.bin -T link.ld --oformat binary kmain.o libprint.o

#Combine the bootsector and kernel into a single image
