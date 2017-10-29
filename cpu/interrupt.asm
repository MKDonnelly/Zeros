; One and only one callback function
; This is the master interrupt handler;
; all interrupts must go through it to
; be routed to the correct function.
[extern main_interrupt_handler]

; Initilize an interrupt of the given
; number. This calls the main interrupt
; handler in isr.c -- int_handler -- which
; routes interrupts to the correct functions.
%macro INTERRUPT_NUMBER 1
global isr%1
isr%1:

   ;push error code if int nunmber <8 || 9 || > 14
   %if %1 < 8 || %1 == 9 || %1 > 14 
     push byte 0
   %endif

   push byte %1 ;;Push the interrupt number

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
%endmacro

; Zero divide trap
INTERRUPT_NUMBER 0

;Testing isr
INTERRUPT_NUMBER 30

INTERRUPT_NUMBER 31

;For timer
INTERRUPT_NUMBER 32

; Handles the PIC keyboard 
; interrupt on IRQ1.
INTERRUPT_NUMBER 33

INTERRUPT_NUMBER 34
INTERRUPT_NUMBER 35
INTERRUPT_NUMBER 36
INTERRUPT_NUMBER 37
INTERRUPT_NUMBER 38
INTERRUPT_NUMBER 39

