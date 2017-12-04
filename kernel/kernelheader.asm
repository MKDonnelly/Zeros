[extern kmain]

PAGE_ALIGN    equ             1 << 0 ;Load kernel on a page boundary
MEMORY_INFO   equ             1 << 1 ;Provide mem info
MODS_INFO     equ             1 << 3

MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS equ PAGE_ALIGN | MEMORY_INFO | MODS_INFO 

[bits 32]

global arch_start

align 4
multiboot_header:
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)


arch_start:
    ;Setup the stack
    ;mov ebp, 0x90000
    ;mov esp, ebp

    push ebx     ;Pass multiboot header
   
    call kmain

stop:
    cli
    hlt
    jmp stop

