[extern kmain]

MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS equ 0

[bits 32]

global arch_start

multiboot_header:
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)


arch_start:
    mov ebp, 0x90000
    mov esp, ebp
   
    call kmain

stop:
    cli
    hlt
    jmp stop

