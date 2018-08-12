#Compiler is gcc
TOOL_DIR := $(HOME)/bin/os/tools64/bin/
CC := $(TOOL_DIR)x86_64-elf-gcc
LD := $(TOOL_DIR)x86_64-elf-ld
ASM := nasm

#Build artifacts
objdir := build
os_image := kernel.elf
kernel := $(objdir)/kernel.elf

#Arch stuff
arch_c_srcs := 				\
	arch/x86/portio.c		\
	arch/x86/drivers/vga3h/vga3h.c	\
	arch/x86/drivers/vgacommon/modeset.c	\
	arch/x86/drivers/vgacommon/vgacommon.c	\
	arch/x86/drivers/vgacommon/vgafont.c	\
	arch/x86/archx64.c		\
	arch/x86/lmode/idt.c		\
	arch/x86/lmode/gdt.c		\
	arch/x86/lmode/isr.c		\
	arch/x86/lmode/paging.c		\
	arch/x86/drivers/lapic.c

arch_asm_srcs :=			\
	arch/x86/cpu.asm		\
	arch/x86/lmode/cpuidasm.asm	\
	arch/x86/lmode/pagingasm.asm	\
	arch/x86/lmode/descriptors.asm	\
	arch/x86/lmode/spinlock.asm	\
	arch/x86/lmode/int.asm

arch_header = arch/x86/lmode/mbheader.asm

arch_src_dirs :=		\
	arch/			\
	arch/x86		\
	arch/x86/drivers/	\
	arch/x86/drivers/vga3h	\
	arch/x86/drivers/vgacommon	\
	arch/x86/drivers/serial	\
	arch/x86/lmode/		

arch_objs = $(arch_c_srcs:%.c=$(objdir)/%.o) $(arch_asm_srcs:%.asm=$(objdir)/%.o)
arch_header_obj = $(arch_header:%.asm=$(objdir)/%.o)

#Driver stuff
driver_srcs := 				\
	drivers/ata/ata_pio.c

driver_src_dirs :=		\
	drivers/ata

driver_objs = $(driver_srcs:%.c=$(objdir)/%.o)


#fs stuff
#fs_srcs :=				\
#	fs/fs.c				\
#	fs/initrd/initrd.c

#fs_src_dirs := fs/initrd

#fs_objs = $(fs_srcs:%.c=$(objdir)/%.o)


#kernel stuff
kernel_srcs :=				\
	kernel/kmain64.c		\
	kernel/mm/heap.c		\
	kernel/mm/bitmap_heap.c		\
	kernel/mm/blocklist_heap.c

kernel_src_dirs := kernel/ kernel/mm/

kernel_objs = $(kernel_srcs:%.c=$(objdir)/%.o)


#Lib stuff
lib_srcs := 				\
	lib/bitwise.c			\
	lib/genericll.c		\
	lib/print.c			\
	lib/random.c			\
	lib/string.c			

lib_src_dirs := lib/

lib_objs = $(lib_srcs:%.c=$(objdir)/%.o)



#Compile and directory stuff
ROOTDIR := .

CFLAGS := -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 
CFLAGS += -ffreestanding -Wall -g
CFLAGS += -I$(ROOTDIR)/include -I. -I$(ROOTDIR)

LDFLAGS = -z max-page-size=0x1000 -T arch/x86/x64_link.ld

ASMFLAGS := -f elf64 -g 

#All directories to create under the build directory
src_dirs := $(arch_src_dirs) $(drivers_src_dirs) $(fs_src_dirs) \
	$(kernel_src_dirs) $(lib_src_dirs)

all: pre-build $(kernel) post-build

pre-build:
	@mkdir -p $(objdir)
	@$(call make-build)
	@\rm ./arch/current_arch
	@ln -s x86/archx64.h ./arch/current_arch

$(kernel): $(arch_header_obj) $(arch_objs) $(drivers_objs) \
	$(fs_objs) $(kernel_objs) $(lib_objs)
	@echo "   LD    $@"
	@$(LD) $(LDFLAGS) $(arch_header_obj) $(arch_objs) $(drivers_objs) \
		$(fs_objs) $(kernel_objs) $(lib_objs)  -o $@

post-build:
	@cp ./build/$(os_image) isofiles/boot/$(os_image)
	@grub-mkrescue isofiles -o os.iso 2> /dev/null

run: all
	@qemu-system-x86_64 os.iso

debug: all
	@qemu-system-x86_64 os.iso -S -s &
	@gdb -q -x .gdbdebug64

clean:
	@\rm -Rf ./build
	@-rm -f os.iso
