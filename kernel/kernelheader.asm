
;Load boot modules on a page boundary
PAGE_ALIGN    equ             1 << 0 

;Provide memory information to kernel
MEMORY_INFO   equ             1 << 1 

;Provide video info to kernel
;(QEMU can't)
;VIDEO_INFO    equ             1 << 2

MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS equ PAGE_ALIGN | MEMORY_INFO ;| VIDEO_INFO 

[bits 32]

global arch_start

align 4
multiboot_header:
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
;    dd 0, 0, 0, 0, 0 ;Zero out the bytes for AOUT_KLUDGE
;    dd 0 ;0 = set graphics mode
;    dd 1024, 768, 16 ;Width, height, and depth for display

KERNEL_STACK_START equ 0x300000

[extern enter_long_mode]
[extern kmain]

arch_start:
    mov ebp, KERNEL_STACK_START
    mov esp, ebp

    ;Pass multiboot header to kmain
    ;mov [mb_temp], ebx

    ;call enter_long_mode

    ;mov ebx, [mb_temp]
    ;push ebx
    ;call kmain
;    call setup_page_tables
;    call enable_paging

;    lgdt [gdt64.pointer]
    ;jmp gdt64.code:long_mode_start

;    mov dword [0xb8000], 0x2f4b2f4f
;    hlt

    ;Make sure to pass the multiboot header
    ;AFTER setting the stack
    push ebx 
    call kmain

stop:
    cli
    hlt
    jmp stop

section .bss
mb_temp: resb 8 
