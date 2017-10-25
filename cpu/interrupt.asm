[extern zero_divide]
[extern keyboard_handler]
[extern default_pic]

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

global isr30
isr30:
   push byte 0
   push byte 30
   SETUP_INT
   call default_pic 
   END_INT

global isr31
isr31:
   push byte 0
   push byte 31
   SETUP_INT
   call default_pic
   END_INT

global isr32
isr32:
   push byte 0
   push byte 32
   SETUP_INT
   call default_pic
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

global isr34
isr34:
   push byte 0
   push byte 34
   SETUP_INT
   call default_pic
   END_INT

global isr35
isr35:
   push byte 0
   push byte 35
   SETUP_INT
   call default_pic
   END_INT

global isr36
isr36:
   push byte 0
   push byte 36
   SETUP_INT
   call default_pic
   END_INT

global isr37
isr37:
   push byte 0
   push byte 37
   SETUP_INT
   call default_pic
   END_INT

global isr38
isr38:
   push byte 0
   push byte 38
   SETUP_INT
   call default_pic
   END_INT

global isr39
isr39:
   push byte 0
   push byte 39
   SETUP_INT
   call default_pic
   END_INT

