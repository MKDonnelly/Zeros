; One and only one callback function
; This is the master interrupt handler;
; all interrupts must go through it to
; be routed to the correct function.
[extern main_interrupt_handler]

; This will be used to map interrupt
; numbers to interrupt handler when
; they are defined
[extern add_idt_entry]


; This is common to all interrupts.
; The only thing that differs is 
; the error code and interrupt
; number pushed by each. By having
; each interrupt jump here, we cut
; down the size of the compiled file.
interrupt_common:
   cli
   pusha
   mov ax, ds  ;Save the data segment
   push eax    ;descriptor
   mov ax, 0x10 
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   call main_interrupt_handler 

   pop eax
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   popa
   add esp, 8
   iret

; This defines an interrupt 
; handler that will be automatically
; called when the cpu triggers the isr. 
%macro DEFINE_INTERRUPT 1
global isr%1
isr%1:
   ;Push an error code if none is
   ;provided by the cpu to keep the
   ;stack constant through each interrupt.
   %if %1 < 8 || %1 == 9 || %1 > 14
      push byte 0
   %endif

   push byte %1
   jmp interrupt_common 
%endmacro


; Initilize every interrupt
; to point to the main 
; interrupt handler
%assign i 0
%rep 100 
   DEFINE_INTERRUPT i
%assign i i+1
%endrep

; This handy routine initilizes
; interrupt #x to point to 
; isr#x. Without this, we would
; have to go
;    add_idt_entry(#x, (u32)isr#x)
; 256 times in c!
global init_idt
init_idt:
   push ebp
   mov ebp, esp
   sub esp, 0x8

%assign j 0
%rep 100
   mov eax, isr %+ j
   sub esp, 0x8
   push eax
   push byte j
   call add_idt_entry
   add esp, 0x10
   %assign j j+1
%endrep

   leave
   ret

global srupdate
srupdate:
   ;Update the segment regesters
   ;after a GDT init
   mov ax, 0x10 ;0x10 is our segment
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov ss, ax
   jmp 0x8:flush
flush:
   ret

