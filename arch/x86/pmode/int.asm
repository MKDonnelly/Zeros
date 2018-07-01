; One and only one callback function
; This is the master interrupt handler;
; all interrupts must go through it to
; be routed to the correct function.
[extern main_interrupt_handler]

; This will be used to map interrupt
; numbers to interrupt handler when
; they are defined
[extern idt_add_entry]

extern gdt_kernel_data
extern gdt_kernel_code

;Stores the current context during an interrupt.
;The scheduler may request this using get_current_context
;and set this using set_current_context. This is the basic
;method for task switching.
current_context dq 0

global get_current_context
get_current_context:
   mov eax, [current_context]
   ret

global set_current_context
set_current_context:
   mov eax, [esp+4]
   mov [current_context], eax
   ret


; This is common to all interrupts.
; The only thing that differs is 
; the error code and interrupt
; number pushed by each. By having
; each interrupt jump here, we cut
; down the size of the compiled file.

; Stack 
;     ;;;;; PUSHED BY CPU ;;;;;;;
;     32 bits EFLAGS
;     32 bits CS
;     32 bits EIP
;
;     ;;;;; PUSHED BY INTERRUPT_COMMON ;;;;;;;;
;     32 bits error code, int #
;     <pushad stuff>
;        32 bits e[a,c,d,b]x, edi, esi, esp, ebp

[extern arch_save_context]
[extern arch_set_context]
interrupt_common:
   pushad

   ;Save segment registers for userland interrupt
   push ds
   push es
   push fs
   push gs

   ;Load in kernel land segment registers
   mov ax, 0x10 ;kernel code
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   ;Store the current context into "current_context"
   ;for use by the scheduler.
   mov [current_context], esp

   ;Save the current context of the stopped task.
   ;This is done so that the scheduler can swap out tasks
   ;if needed.
   call arch_save_context

   ;TODO explicitly pass in context_t as a pointer
   call main_interrupt_handler 

   ;Restore the context. This may be different than the
   ;context we saved if the scheduler switched tasks.
   call arch_set_context

   ;Restore context after scheduler call
   mov esp, [current_context]

   ;Restore userland segment registers
   pop gs
   pop fs
   pop es
   pop ds

   ;Restore register state
   popad

   ;This is to move the stack back
   ;over the error code and interrupt number.
   ;While we only push two bytes worth of
   ;data, the stack is automatically aligned
   ;to 4-byte boundaries. As a result, we need
   ;to jump back 8 bytes. 
   add esp, 8
   iretd


; This defines an interrupt 
; handler that will be automatically
; called when the cpu triggers the isr. 
%macro DEFINE_INTERRUPT 1
isr%1:
   ;Push an error code if none is
   ;provided by the cpu to keep the
   ;stack constant through each interrupt.
   %if %1 < 8 || %1 == 9 || %1 > 14
      push dword 0
   %endif

   push dword %1
   jmp interrupt_common 
%endmacro


; Initilize every interrupt
; to point to the main 
; interrupt handler
%assign i 0
%rep 256
   DEFINE_INTERRUPT i
%assign i i+1
%endrep

; This handy routine initilizes
; interrupt #x to point to 
; isr#x. Without this, we would
; have to go
;    idt_add_entry(#x, isr#x)
; 256 times in c!
global idt_setup_isrs
idt_setup_isrs:
   push ebp
   mov ebp, esp

%assign j 0
%rep 256
   ;Get address of j'th ISR
   mov eax, isr %+ j

   ;Push the address of the j'th ISR and
   ;the interrupt number that it handles
   push eax
   push dword j
   
   ;Add the entry to the IDT
   call idt_add_entry

   ;Remove the two values pushed onto the stack
   add esp, 0x8

   %assign j j+1
%endrep

   mov esp, ebp
   pop ebp
   ret
