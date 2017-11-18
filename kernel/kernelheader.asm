[extern kmain]

PAGE_ALIGN equ             1 << 0
MEMORY_INFORMATION equ     1 << 1

MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS equ PAGE_ALIGN | MEMORY_INFORMATION

[bits 32]

global arch_start

multiboot_header:
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)


arch_start:
    push ebx     ;Pass multiboot header

    ;Setup the stack
    mov ebp, 0x90000
    mov esp, ebp
   
    call kmain

stop:
    cli
    hlt
    jmp stop

