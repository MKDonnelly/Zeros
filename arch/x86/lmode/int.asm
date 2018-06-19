
interrupt_common:
   iret

%macro DEFINE_INTERRUPT 1
global isr%1
isr%1:
   push qword %1
   jmp interrupt_common
%endmacro
