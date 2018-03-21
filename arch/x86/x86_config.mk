export CFLAGS = -fno-pie -m32 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -g -I$(ROOTDIR)/include -I. -I$(ROOTDIR)
export LDFLAGS = -m elf_i386 -T arch/x86/x86_link.ld
export NASMFLAGS = -f elf32
export KERNEL_HEADER = kernelheader.o 
