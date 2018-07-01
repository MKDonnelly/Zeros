;assembly routines related to descriptors

global idt_load
idt_load:
   lidt [rdi]
   ret

; Why ljmp_to is needed:
; There are a few times where it would be really
; nice to do a long jump to a register. However,
; this will not work. instead, this can be accomplished
; by setting up a stack and ireting from it
%macro LJMP_TO 2
   pushf    ;eflags
   push qword %1  ;segment
   push %2  ;new eip
   iret
%endmacro

global gdt_load
gdt_load:
   lgdt [rdi]

   mov eax, 0x8
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov ss, ax

   mov rax, rsp
   push qword 0x8
   push rax
   push 2
   push qword 0x8
   push .return_gdt

.return_gdt:
   ret
