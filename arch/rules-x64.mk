#Compiler is gcc
CC := gcc
LD := ld
ASM := nasm
OBJDUMP := objdump
OBJCOPY := objcopy
AR := ar

#Build artifacts
objdir := build
os_image := kernel.elf
kernel := $(objdir)/kernel.elf

#Arch stuff
arch_c_srcs := 				\
	arch/x86/lmode/kmain64.c

arch_asm_srcs :=			\
	arch/x86/lmode/kernelheader.asm

arch_src_dirs :=		\
	arch/			\
	arch/x86		\
	arch/x86/lmode/		

arch_c_objs = $(arch_c_srcs:%.c=$(objdir)/%.o)
arch_asm_objs = $(arch_asm_srcs:%.asm=$(objdir)/%.o)


#Lib stuff
lib_c_srcs := lib/bitwise.c			

lib_src_dirs := lib/

lib_c_objs = $(lib_c_srcs:%.c=$(objdir)/%.o)	



#Compile and directory stuff
ROOTDIR := .

CFLAGS := -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 
CFLAGS += -fno-pie -m64 -ffreestanding -fno-stack-protector -nostdlib 
CFLAGS += -nostdinc -fno-builtin -Wall -g
CFLAGS += -I$(ROOTDIR)/include -I. -I$(ROOTDIR)

LDFLAGS = -m elf_x86_64 -z max-page-size=0x1000
LDSCRIPT = -T arch/x86/x64_link.ld

ASMFLAGS := -f elf64 -g

#All directories to create under the build directory
src_dirs := $(arch_src_dirs) $(lib_src_dirs)

all: pre-build $(kernel) post-build

pre-build:
	@mkdir -p $(objdir)
	@$(call make-build)

$(kernel): $(arch_asm_objs) $(arch_c_objs) $(lib_c_objs)
	@echo "   LD    $@"
	@$(LD) $(LDFLAGS) $(LDSCRIPT) $(arch_asm_objs) $(arch_c_objs) -o $@

post-build:
	@cp ./build/$(os_image) isofiles/boot/$(os_image)
	@grub-mkrescue isofiles -o os.iso 2> /dev/null

run: all
	@qemu-system-x86_64 os.iso

clean:
	@\rm -Rf ./build
	@-rm -f os.iso
