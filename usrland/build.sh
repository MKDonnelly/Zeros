#/bin/bash

nasm -f elf32 stdlib-asm.asm -o stdlib-asm.o
gcc -nostdlib -Wall -g -m32 -c test.c -o test.o
ld -T ./userland_link.ld -m elf_i386 test.o stdlib-asm.o -o test
#../../tools/bin/i686-elf-gcc -nostdlib test.c stdlib-asm.o -o test
