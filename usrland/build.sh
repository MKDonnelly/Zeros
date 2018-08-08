#/bin/bash

nasm -f elf32 stdlib-asm.asm -o stdlib-asm.o
../../tools/bin/i686-elf-gcc -nostdlib test.c stdlib-asm.o -o test
