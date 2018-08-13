; Assembly routines to deal with interrupts

%macro push_regs 0
   push rax
   push rcx
   push rdx
   push rbx
   push rbp
   push rsi
   push rdi
   push r8
   push r9
   push r10
   push r11
   push r12
   push r13
   push r14
   push r15
%endmacro

%macro pop_regs 0
   pop r15
   pop r14
   pop r13
   pop r12
   pop r11
   pop r10
   pop r9
   pop r8
   pop rdi
   pop rsi
   pop rbp
   pop rbx
   pop rdx
   pop rcx
   pop rax
%endmacro

[extern main_interrupt_handler]
interrupt_common:

   ;Save general purpose registers
   push_regs

   call main_interrupt_handler

   pop_regs

   ;Remove error code and interrupt number
   add esp, 0x10

   iretq


%macro DEFINE_INTERRUPT 1
global isr%1
isr%1:
   ;These are the interrupt vectors that push error codes.
   ;All others must push a 0 to have a consistent stack frame.
   %if %1 != 8 && %1 != 10 && %1 != 11 && \
       %1 != 12 && %1 != 13 && %1 != 15 && %1 != 17
      push byte 0
   %endif
   push qword %1
   jmp interrupt_common
%endmacro

%assign i 0
%rep 100
   DEFINE_INTERRUPT i
%assign i i+1
%endrep

[extern idt_add_entry]
global idt_setup_isrs
idt_setup_isrs:
   push rbp
   mov rbp, rsp

%assign j 0
%rep 100
   ;Get address of j'th ISR
   mov rax, isr %+ j

   ;Set the address of j'th ISR and
   ;the interrupt number that it handles
   mov rdi, j
   mov rsi, rax
   
   ;Add the entry to the IDT
   call idt_add_entry
   %assign j j+1
%endrep

   mov rsp, rbp
   pop rbp
   ret
