
ARCH ?= x86

CFLAGS = -fno-pie -m32 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -g -Iinclude/ -I.

ASMFILES = $(shell find . -name '*\.asm' )
ASMOUTPUT = $(ASMFILES:.asm:.o)
CFILES = $(shell find . -name '*\.c' )
COUTPUT = $(CFILES:.c:.o)

arch_all: $(ASMOUTPUT) $(COUTPUT)


