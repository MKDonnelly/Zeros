
; Used to load a TSS into the TR register
; TSS must be in GDT before using
global load_tss
load_tss:
   mov ax, 0x2B
   ltr ax
   ret

; Used to load the IDT
; Takes a single pointer to the
; IDT as an argument. Currently not working
global load_idt_asm
load_idt_asm:
   mov eax, [esp+4]
   lidt [eax]

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

; Thread related code

; Jump to the specified code. Accepts
; A single argument of thread_context_t*
; to jump to 
global arch_jump_to_thread
arch_jump_to_thread:
   mov esp, [esp+4]
   popa

   ; In our interrupt code, we push an error
   ; code and interrupt number before pushing
   ; the GPRs. This removes them.
   add esp, 8
   ret

; This routine switches the cpu to usermode.
; Code after executing this will be in userland.
; TODO have function to jump back to instead of
;      running off the end.
global arch_userland_jump
arch_userland_jump:
   cli
   mov ax, 0x23
   mov ds, ax
   mov es, ax 
   mov fs, ax
   mov gs, ax
   mov eax, esp
   push 0x23
   push eax
   pushf
   push 0x1B
   push here
   iret
here:
   nop



