section .text
global _start
_start:
   mov eax, 2
   mov ebx, 0
   mov ecx, str
   mov edx, str_len
   int 0x31

   ;open file
   mov eax, 0
   mov ebx, fname
   int 0x31
 
   ;read from file
   mov eax, 4
   mov ebx, 1
   mov ecx, filebuf
   mov edx, 9
   int 0x31

   ;write to stdout
   mov eax, 2
   mov ebx, 0
   mov ecx, filebuf
   mov edx, 9
   int 0x31

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

section .data
fname: db 'data.txt', 0
str: db 'Hello', 0
str_len: equ $-str
tmp: db ' '
filebuf: db '          '



