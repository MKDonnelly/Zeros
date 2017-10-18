[extern isr_handler]

isr_common_stub:
   pusha
   mov ax, ds
   push eax
   mov ax, 0x10
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax

   call isr_handler

   pop eax
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   popa
   add esp, 8
   sti
   iret

global isr2
isr2:
   ;call isr_handler
   ;hlt
   cli
   push byte 0
   push byte 2
   jmp isr_common_stub

