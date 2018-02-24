
global tss_flush
tss_flush:
   mov ax, 0x2B
   ltr ax
   ret

[extern test_userland]

global userland_jump
userland_jump:
   cli
   mov ax,0x23
   mov ds, ax
   mov es, ax 
   mov fs, ax
   mov gs, ax
   mov eax, esp
   push 0x23
   push eax
   pushf
   push 0x1B
   push here
   iret
here:
   nop

