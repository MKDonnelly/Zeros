#Compiler is gcc
CC := gcc
LD := ld
ASM := nasm
OBJCOPY := objcopy

#Build artifacts
objdir := build
os_image := kernel.elf
kernel := $(objdir)/kernel.elf


#UEFI header stuff
uefi_header := arch/x86/lmode/main.c
uefi_header_obj := $(uefi_header:%.c=$(objdir)/%.o)
uefi_header_so := $(uefi_header:%.c=$(objdir)/%.so)
uefi_header_efi := $(uefi_header:%.c=$(objdir)/%.efi)
uefi_header_dir := arch/x86/lmode/

UEFI_CFLAGS := -I/usr/include/efi -I/usr/include/efi/x86_64 
UEFI_CFLAGS += -I/usr/include/efi/protocol -fno-stack-protector
UEFI_CFLAGS += -fPIC -fshort-wchar -mno-red-zone -DHAVE_USE_MS_ABI -c

UEFI_LD_FLAGS := -nostdlib -znocombreloc -T /usr/lib/elf_x86_64_efi.lds
UEFI_LD_FLAGS += -shared -Bsymbolic -L /usr/lib /usr/lib/crt0-efi-x86_64.o
#UEFI_LD_FLAGS += -lefi -lgnuefi

UEFI_OBJCOPY_FLAGS := -j .text -j .sdata -j .data -j .dynamic -j .dynsym
UEFI_OBJCOPY_FLAGS += -j .rel -j .rela -j .reloc --target=efi-app-x86_64


src_dirs := $(uefi_header_dir)
all: pre-build $(uefi_header_efi)

pre-build:
	@mkdir -p $(objdir) 
	@$(call make-build)

$(uefi_header_efi): 
	@$(CC) $(UEFI_CFLAGS) $(uefi_header) -o $(uefi_header_obj)
	@$(LD) $(UEFI_LD_FLAGS) $(uefi_header_obj) -o $(uefi_header_so) -lefi -lgnuefi
	@$(OBJCOPY) $(UEFI_OBJCOPY_FLAGS) $(uefi_header_so) $(uefi_header_efi)

run: pre-build
	@qemu-system-x86_64 -cpu qemu64 -L /usr/share/OVMF/ -bios /usr/share/qemu/OVMF.fd -hda uefi.img

clean:
	@\rm -Rf build/
