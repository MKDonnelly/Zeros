ARCH ?= x86
ARCH_CONFIG = ./config.ini
include $(ARCH_CONFIG)

ASMFILES = $(shell find . -name '*\.asm')
CFILES   = $(shell find . -name '*\.c')
COBJS    = $(CFILES:.c=.o)
ASMOBJS  = $(ASMFILES:.asm=.o)

BUILDDIR = ../../build/

all: $(COBJS) $(ASMOBJS)
	

%.o: %.asm
	@-mkdir -p $(BUILDDIR)/$(dir $@)
	@nasm -f elf32 -o $(BUILDDIR)/$@ $<

.c.o:
	@-mkdir -p $(BUILDDIR)/$(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $(BUILDDIR)/$@


