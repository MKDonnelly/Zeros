;assembly routines related to descriptors

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
   ;No need to do a long jump if the CS does not change
   lgdt [rax]
   ret
