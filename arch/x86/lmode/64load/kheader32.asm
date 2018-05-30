;32 Bit kernel. The purpose of this is to load
;the 64 bit kernel contained in the .kernel64 section

[bits 32]

;Load boot modules on a page boundary
PAGE_ALIGN    equ             1 << 0 

;Provide memory information to kernel
MEMORY_INFO   equ             1 << 1 

;Provide video info to kernel
;(QEMU can't)
;VIDEO_INFO    equ             1 << 2

MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS equ PAGE_ALIGN | MEMORY_INFO ;| VIDEO_INFO 

global _start

align 4
multiboot_header:
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
;    dd 0, 0, 0, 0, 0 ;Zero out the bytes for AOUT_KLUDGE
;    dd 0 ;0 = set graphics mode
;    dd 1024, 768, 16 ;Width, height, and depth for display


[extern ldscript_kernel64]
[extern get_kernel64_addr]
_start:
    mov ebp, stack_top 
    mov esp, ebp

    ;push dword [ldscript_kernel64]
    mov eax, ldscript_kernel64
    call get_kernel64_addr
    ;call long_mode_jump

stop:
    cli
    hlt
    jmp stop

%macro LJMP_TO 2
   pushf    ;eflags
   push dword %1  ;segment
   push dword %2  ;new eip
   iret
%endmacro


global long_mode_jump
[extern kmain]
long_mode_jump:
   call setup_page_tables
   call enable_paging

   ;Setup global descriptor table
   ;and jump to the kernel code
   lgdt [gdt64.pointer]

   ;TODO: Cannot long jump to reg, so
   ; we will do and iret instead to load
   ; the segment:offset 
   ;mov eax, [kmain_start_addr]
;   LJMP_TO gdt64.code, eax
.return:
   ret

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
   cmp ecx, 512
   jne .map_p2_table

   ret

;;;;;; TEMPORARY FOR LONG MODE TEST
section .bss
align 4096
p4_table:
   resb 4096
p3_table:
   resb 4096
p2_table:
   resb 4096
stack_bottom:
   resb 1024
stack_top:

section .rodata
align 8
gdt64:
   dq 0
.code: equ $ - gdt64
   dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
.pointer:
   dw $ - gdt64 - 1
   dq gdt64

