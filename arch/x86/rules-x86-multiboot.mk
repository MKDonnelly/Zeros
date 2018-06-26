#Compiler is gcc
CC := gcc
LD := ld
ASM := nasm

#Build artifacts
objdir := build
os_image := kernel.elf
kernel := $(objdir)/kernel.elf

#Arch stuff
arch_c_srcs := 				\
	arch/x86/archx86.c		\
	arch/x86/portio.c		\
	arch/x86/drivers/cmos.c		\
	arch/x86/drivers/keyboard.c	\
	arch/x86/drivers/mouse.c	\
	arch/x86/drivers/pic.c		\
	arch/x86/drivers/timer.c	\
	arch/x86/drivers/serial/serial.c	\
	arch/x86/drivers/vga3h/vga3h.c	\
	arch/x86/drivers/vgacommon/modeset.c	\
	arch/x86/drivers/vgacommon/vgacommon.c	\
	arch/x86/drivers/vgacommon/vgafont.c	\
	arch/x86/pmode/arch_task.c	\
	arch/x86/pmode/context.c	\
	arch/x86/pmode/frame.c		\
	arch/x86/pmode/gdt.c		\
	arch/x86/pmode/idt.c		\
	arch/x86/pmode/isr.c		\
	arch/x86/pmode/loader.c		\
	arch/x86/pmode/paging.c		\
	arch/x86/pmode/schedule.c	\
	arch/x86/pmode/syscall.c	\
	arch/x86/pmode/tss.c

arch_asm_srcs :=			\
	arch/x86/cpu.asm		\
	arch/x86/pmode/descriptors.asm	\
	arch/x86/pmode/int.asm		\
	arch/x86/pmode/pagingasm.asm	\
	arch/x86/pmode/spinlock.asm	\
	arch/x86/pmode/tsc.asm

arch_header := arch/x86/pmode/mbheader.asm

arch_src_dirs :=		\
	arch/x86/pmode/		\
	arch/x86/drivers/	\
	arch/x86/drivers/serial	\
	arch/x86/drivers/vga3h	\
	arch/x86/drivers/vgacommon

arch_objs = $(arch_c_srcs:%.c=$(objdir)/%.o) \
		$(arch_asm_srcs:%.asm=$(objdir)/%.o)
arch_header_obj = $(arch_header:%.asm=$(objdir)/%.o)


#Driver stuff
driver_srcs := 		\
	drivers/ata/ata_pio.c	\
	drivers/pci/pci.c	\
	drivers/pci/pci_map.c	\
	drivers/net/rtl8139.c

driver_src_dirs := 	\
	drivers/ata/	\
	drivers/pci/	\
	drivers/net/

driver_objs = $(driver_srcs:%.c=$(objdir)/%.o)


#fs stuff
fs_srcs :=			\
	fs/vfs/vnode.c		\
	fs/mbr.c		\
	fs/vfs/fsmanager.c	\
	fs/zsfs/zsfs.c		\
	fs/initrd/initrd.c	

fs_src_dirs := fs/initrd fs/zsfs fs/vfs

fs_objs := $(fs_srcs:%.c=$(objdir)/%.o)


#kernel stuff
kernel_srcs := 		\
	kernel/kmain.c		\
	kernel/mm/heap.c	\
	kernel/mm/heap_bitmap.c	\
	kernel/mm/heap_blocklist.c	\
	kernel/sched/round_robin.c	\
	kernel/sched/sched.c	\
	kernel/sched/workqueue.c	\
	kernel/syscall.c	\
	kernel/task.c		\
	kernel/blkdev/blkdev.c	\
	kernel/blkdev/drive.c

kernel_src_dirs := 		\
	kernel/mm		\
	kernel/sched		\
	kernel/blkdev

kernel_objs := $(kernel_srcs:%.c=$(objdir)/%.o)


#Lib stuff
lib_srcs := 			\
	lib/bitwise.c		\
	lib/generic_ll.c	\
	lib/keyboard.c		\
	lib/print.c		\
	lib/random.c		\
	lib/string.c		\
	lib/timing.c

lib_src_dirs := lib/

lib_objs = $(lib_srcs:%.c=$(objdir)/%.o)	



#Compile and directory stuff
ROOTDIR := .

CFLAGS := -fno-pie -m32 -ffreestanding -fno-stack-protector -nostdlib 
CFLAGS += -nostdinc -fno-builtin -Wall -g
CFLAGS += -I$(ROOTDIR)/include -I. -I$(ROOTDIR)

LDFLAGS = -m elf_i386 -T arch/x86/x86_link.ld

ASMFLAGS := -f elf32 -g 

#All directories to create under the build directory
src_dirs := $(arch_src_dirs) $(driver_src_dirs) $(fs_src_dirs) \
		$(kernel_src_dirs) $(lib_src_dirs)

all: pre-build $(kernel) post-build

pre-build:
	@mkdir -p $(objdir)
	@$(call make-build)

$(kernel): $(arch_objs) $(driver_objs) \
		$(fs_objs) $(kernel_objs) $(lib_objs) $(arch_header_obj)
	@echo "   LD    $@"
	@$(LD) $(LDFLAGS) $(arch_header_obj) $(arch_objs) 	\
		$(driver_objs) $(fs_objs) $(kernel_objs) 	\
		$(lib_objs) -o $@

post-build:
	@cp ./build/$(os_image) isofiles/boot/$(os_image)
	@grub-mkrescue isofiles -o os.iso 2> /dev/null

run: all
	@qemu-system-x86_64 os.iso -drive format=raw,file=./testdisk -net nic,model=rtl8139

debug: all
	@qemu-system-x86_64 os.iso -drive format=raw,file=./testdisk -S -s &
	@gdb -x .gdbdebug

clean:
	@\rm -Rf ./build
	@-rm -f os.iso
