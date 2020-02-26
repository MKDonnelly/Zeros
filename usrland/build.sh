#/bin/bash

\rm shell shell.o stdlib-asm.o echo echo.o pwd.o pwd 
\rm rinput rinput.o sar sar.o ls.o ls
\rm mkdir mkdir.o touch touch.o rm rm.o cat cat.o

nasm -f elf32 stdlib-asm.asm -o stdlib-asm.o

gcc -nostdlib -Wall -g -m32 -c stdlib.c -o stdlib.o

gcc -nostdlib -Wall -g -m32 -c shell.c -o shell.o
gcc -nostdlib -Wall -g -m32 -c echo.c -o echo.o
gcc -nostdlib -Wall -g -m32 -c pwd.c -o pwd.o
gcc -nostdlib -Wall -g -m32 -c rinput.c -o rinput.o
gcc -nostdlib -Wall -g -m32 -c sar.c -o sar.o
gcc -nostdlib -Wall -g -m32 -c ls.c -o ls.o
gcc -nostdlib -Wall -g -m32 -c mkdir.c -o mkdir.o
gcc -nostdlib -Wall -g -m32 -c touch.c -o touch.o
gcc -nostdlib -Wall -g -m32 -c rm.c -o rm.o
gcc -nostdlib -Wall -g -m32 -c cat.c -o cat.o

ld -T ./userland_link.ld -m elf_i386 shell.o stdlib.o stdlib-asm.o -o shell
ld -T ./userland_link.ld -m elf_i386 echo.o stdlib.o stdlib-asm.o -o echo
ld -T ./userland_link.ld -m elf_i386 pwd.o stdlib.o stdlib-asm.o -o pwd
ld -T ./userland_link.ld -m elf_i386 rinput.o stdlib.o stdlib-asm.o -o rinput
ld -T ./userland_link.ld -m elf_i386 sar.o stdlib.o stdlib-asm.o -o sar
ld -T ./userland_link.ld -m elf_i386 ls.o stdlib.o stdlib-asm.o -o ls
ld -T ./userland_link.ld -m elf_i386 mkdir.o stdlib.o stdlib-asm.o -o mkdir
ld -T ./userland_link.ld -m elf_i386 touch.o stdlib.o stdlib-asm.o -o touch
ld -T ./userland_link.ld -m elf_i386 rm.o stdlib.o stdlib-asm.o -o rm
ld -T ./userland_link.ld -m elf_i386 cat.o stdlib.o stdlib-asm.o -o cat

\rm ../arch/x86/initrd
./makeinitrd ../arch/x86/initrd shell echo pwd rinput sar ls mkdir touch rm cat mydata
