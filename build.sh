#!/bin/bash

#Compile c kernel
gcc -ffreestanding -c kernel.c -o kernel.o
ld -o kernel.bin -Ttext 0x1000 kernel.o --oformat binary

#compile boot sector assembly
nasm bootsec.asm -f bin -o bootsec.bin 

cat bootsec.bin kernel.bin > osimage.img
