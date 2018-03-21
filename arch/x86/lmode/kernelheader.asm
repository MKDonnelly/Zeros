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

[extern long_mode_jump]
[extern kmain_64]

arch_start:
    mov ebp, KERNEL_STACK_START
    mov esp, ebp

    call long_mode_jump

[bits 64]
    ;Make sure to pass the multiboot header
    ;AFTER setting the stack
    call kmain_64

stop:
    cli
    hlt
    jmp stop

temp dd 0
