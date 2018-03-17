[bits 32]

global enter_long_mode
enter_long_mode:
   call setup_page_tables
   call enable_paging

   lgdt [gdt64.pointer]
   jmp gdt64.code:.return
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
   or eax, 0b11 ; present + rw
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
   cmp ecx, 3
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

section .rodata
gdt64:
   dq 0
.code: equ $ - gdt64
   dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
.pointer:
   dw $ - gdt64 - 1
   dq gdt64
