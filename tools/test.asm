section .data
str: db 'Hello', 0
str_len: equ $-str

section .text

global _start
_start:
   mov ecx, str
   call write_str
   mov eax, 1
   int 0x31

;ecx has address of string
global write_str
write_str:
   mov eax, 0x0
.loop:
   cmp byte [ecx], 0
   je .end
   mov bl, byte [ecx]
   int 0x31
   inc ecx
   jmp .loop
.end:
   ret

