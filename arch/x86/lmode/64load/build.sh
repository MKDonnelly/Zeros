#!/bin/bash

#The image will have the 32 bit header to set everything up,
#along with the 64 bit image cated onto it

#Build kernel64.elf
nasm -f elf64 -g -o kernel64.o kernel64.asm
#gcc -fno-pie -m64 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -c -g kmain.c -o kmain.o 
#max-pagesize is needed to prevent a large binary
ld -T link64.ld -z max-page-size=0x1000 -o kernel64.elf kernel64.o 

#Build khealper
gcc -c -fno-pie -m32 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -g khelper.c -o khelper.o -I /home/me/bin/bos/zeros/include/

#Build the 32 bit header
nasm -f elf32 -g -o kheader32.o kheader32.asm
ld -m elf_i386 -T link32.ld -o kernel32.elf kheader32.o khelper.o

#Build the final image
cat kernel32.elf kernel64.elf > kernel.elf
