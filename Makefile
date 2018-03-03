
#Default target is x86
ARCH ?= x86

ARCH_CONFIG = arch/$(ARCH)/config.ini

include $(ARCH_CONFIG)

KERNEL_NAME = $(OS_NAME).elf
BINFILE = $(BUILDDIR)/$(KERNEL_NAME)

CFILES = $(shell find . -name '*\.c' -not -path "./tools/*")
COUTPUT = $(CFILES:.c=.o)
ASMFILES = $(shell find . -name '*\.asm' -not -path "./tools/*")
ASMOUTPUT = $(ASMFILES:.asm=.o)

all: $(COUTPUT) $(ASMOUTPUT) link

#NOTE: We make sure the asm files are first, since the multiboot
#header needs to be at the head
link: $(COUTPUT) $(ASMOUTPUT)
	@ld -m elf_i386 -o $(BUILDDIR)/$(KERNEL_NAME) -T arch/$(ARCH)/link.ld $(addprefix $(BUILDDIR)/,$(ASMOUTPUT)) $(addprefix $(BUILDDIR)/,$(COUTPUT))

%.o: %.asm
	@-mkdir -p $(BUILDDIR)/$(dir $@)
	@nasm -f elf32 -o $(BUILDDIR)/$@ $<

.c.o: 
	@-mkdir -p $(BUILDDIR)/$(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $(BUILDDIR)/$@

clean:
	@-\rm -Rf ./build/*

run: all
	@qemu-system-x86_64 -kernel build/Zeros.elf -append arg1 -initrd arch/x86/initrd &

debug:
	@qemu-system-x86_64 -serial file:serial.log -kernel build/Zeros.elf  -append arg1 -initrd ./arch/x86/initrd -S -s &
	@gdb -q -x .gdbdebug
