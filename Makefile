
BUILDDIR = $(shell realpath ./build)
ROOTDIR = $(shell realpath .)
include arch/x86/x86_config.mk

all: build_arch build_kernel build_fs build_lib build_staging link 

link:
	@echo $(shell find $(BUILDDIR) -name '*\.o' -and ! -name $(KERNEL_HEADER) ) > /dev/null
	@echo $(shell find $(BUILDDIR) -name $(KERNEL_HEADER) ) > /dev/null
	@ld $(LDFLAGS) -o $(BUILDDIR)/Zeros.elf $(shell find $(BUILDDIR) -name $(KERNEL_HEADER)) $(shell find $(BUILDDIR) -name '*\.o' -and ! -name $(KERNEL_HEADER))
	

build_arch:
	@PREFIX=$(BUILDDIR)/arch/x86 make -C arch/x86 --no-print-directory 

build_kernel:
	@PREFIX=$(BUILDDIR)/kernel/ make -C kernel/ --no-print-directory

build_fs:
	@PREFIX=$(BUILDDIR)/fs/ make -C fs/ --no-print-directory

build_lib:
	@PREFIX=$(BUILDDIR)/lib/ make -C lib/ --no-print-directory

build_staging:
	@PREFIX=$(BUILDDIR)/staging/ make -C staging/ --no-print-directory

run: all
	@qemu-system-x86_64 -kernel build/Zeros.elf -append kernel_cmdline_argunments -initrd arch/x86/initrd &

debug: all
	@qemu-system-x86_64 -kernel build/Zeros.elf -append arg1 -initrd arch/x86/initrd -S -s &
	@gdb -q -x .gdbdebug

clean:
	@\rm -Rf ./build/
