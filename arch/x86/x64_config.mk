export CFLAGS = -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -fno-pie -m64 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -fno-builtin -Wall -g -I$(ROOTDIR)/include -I. -I$(ROOTDIR)
export LDFLAGS = -m elf_x86_64 -T arch/x86/x64_link.ld
export NASMFLAGS = -f elf64
export KERNEL_HEADER = kernelheader.o 
