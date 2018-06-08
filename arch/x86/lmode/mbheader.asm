[bits 32]

section .text
;Load boot modules on a page boundary
PAGE_ALIGN    equ             1 << 0 

;Provide memory information to kernel
MEMORY_INFO   equ             1 << 1 

;Provide video info to kernel
;(QEMU can't)
;VIDEO_INFO    equ             1 << 2

MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS equ PAGE_ALIGN | MEMORY_INFO ;| VIDEO_INFO 

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

arch_start:
    ; setup the kernel stack
    mov ebp, KERNEL_STACK_START
    mov esp, ebp

    ;jump to long mode
    call long_mode_jump

    ;we should never get here


global long_mode_jump
long_mode_jump:
   call setup_page_tables
   call enable_paging

   lgdt [gdt64.pointer]
   jmp gdt64.code:lmode_entry
   ;never returns

enable_paging:
   ;Load p4 to cr3 register for use
   mov eax, p4_table
   mov cr3, eax

   mov eax, cr4
   or eax, 1 << 5
   mov cr4, eax

   mov ecx, 0xC0000080
   rdmsr
   or eax, 1 << 8
   wrmsr

   mov eax, cr0
   or eax, 1 << 31
   mov cr0, eax

   ret

setup_page_tables:
   ;map first p4 entry to p3 table
   mov eax, p3_table
   or eax, 0b11 
   mov [p4_table], eax

   ;map first p3 entry to p2 table
   mov eax, p2_table
   or eax, 0b11
   mov [p3_table], eax

   mov ecx, 0

 .map_p2_table:
   ; map each p2 entry to a hugh page that starts at address 2MiB*ecx
   mov eax, 0x200000
   mul ecx
   or eax, 0b10000011
   mov [p2_table + ecx * 8], eax

   inc ecx
   cmp ecx, 3 ;Map only 3 2MB pages
   jne .map_p2_table

   ret

section .text
[bits 64]
[extern kmain64]
lmode_entry:
   ;Zero out the segment descriptors
   mov ax, 0
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov ss, ax

   call kmain64
.stop:
   cli
   hlt
   jmp .stop


;Temporary page table and GDT entry for long mode entry
section .bss
align 4096
p4_table:
   resb 4096
p3_table:
   resb 4096
p2_table:
   resb 4096

section .rodata
global gdt64
gdt64:
   dq 0
.code: equ $ - gdt64
   dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
.pointer:
   dw $ - gdt64 - 1
   dq gdt64
