[extern zero_divide]
[extern keyboard_handler]

%macro SETUP_INT 0
   cli
   pusha
   mov ax, ds
   push eax
   mov ax, 0x10
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
%endmacro

%macro END_INT 0
   pop eax
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   popa
   add esp, 8
   sti
   iret
%endmacro
   
; <int_handler> is defined in isr.c
; For each interrupt:
; global <name>
; <name>:
;   push byte 0 (error code if int nunmber <8 || 9 || > 14
;   push byte <int #>
;   SETUP_INT
;   call <int_handler>
;   END_INT


; ISR 0, divide by zero trap
global isr0 
isr0:
   push byte 0
   push byte 0
   SETUP_INT
   call zero_divide
   END_INT

; Handles the PIC keyboard 
; interrupt on IRQ1.
global isr33
isr33:
   push byte 0
   push byte 33
   SETUP_INT
   call keyboard_handler
   END_INT
