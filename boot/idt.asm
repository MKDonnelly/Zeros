[bits 32]

global keyboard_handler
global load_idt

extern keyboard_handler_main

load_idt:
   mov edx, [esp + 4]
   lidt [edx]
   sti
   ret

keyboard_handler:
   call keyboard_handler_main
   iretd



