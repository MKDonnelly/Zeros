; Assembly routines for managing the GDT,
; IDT, and TSS 

; Used to load a TSS into the TR register
; TSS must be in GDT before using
global tss_load
tss_load:
   ; See gdt.h for what 0x2B is for
   mov ax, 0x2B
   ltr ax
   ret

; Used to load the IDT
; Takes a single pointer to the
; IDT as an argument. 
global idt_load
idt_load:
   mov eax, [esp+4]
   lidt [eax]
   ret

; Load the gdt and update the segment registers
; constants defined in gdt.c
[extern gdt_kernel_data]
[extern gdt_kernel_code]

; Why ljmp_to is needed:
; There are a few times where it would be really
; nice to do a long jump to a register. However,
; this will not work. instead, this can be accomplished
; by setting up a stack and ireting from it
%macro LJMP_TO 2
   pushf    ;eflags
   push dword %1  ;segment
   push %2  ;new eip
   iret
%endmacro
  

global gdt_load
gdt_load:
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

   ;Do a long jump to activate the GDT
   LJMP_TO [gdt_kernel_code], .gdt_return

.gdt_return:
   ret
