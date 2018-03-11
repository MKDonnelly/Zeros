; Assembly routines for managing the GDT,
; IDT, and TSS 

; Used to load a TSS into the TR register
; TSS must be in GDT before using
global load_tss
load_tss:
   ; See gdt.h for what 0x2B is for
   mov ax, 0x2B
   ltr ax
   ret

; Used to load the IDT
; Takes a single pointer to the
; IDT as an argument. 
global arch_load_idt
arch_load_idt:
   push eax

   mov eax, [esp+8]
   lidt [eax]

   pop eax
   ret

; Load the gdt and update the segment registers
; constants defined in gdt.c
[extern gdt_kernel_data]
[extern gdt_kernel_code]

global load_gdt
load_gdt:
   mov eax, [esp+4]
   lgdt [eax]

   ;We need the [] around the constants
   ;since they are treated as an address
   ;during link time, not like a constant.
   mov ax, [gdt_kernel_data]
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov ss, ax
   ; 0x08 is gdt_kernel_code
   ; TODO try to put constant in
   jmp 0x08:.gdt_return
.gdt_return:
   ret
