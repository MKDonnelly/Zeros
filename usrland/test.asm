section .data
str: db 'Hello', 0
str_len: equ $-str
my_pid: db 1

section .text

global _start
_start:
   mov eax, 2
   mov ebx, 0
   mov ecx, str
   mov edx, str_len
   int 0x31

   mov eax, 3
   int 0x31

   add eax, '0'
   mov byte [my_pid], al
   
   mov eax, 2
   mov ebx, 0
   mov ecx, my_pid
   mov edx, 1
   int 0x31

;   mov ecx, str
;   call write_str

.end:
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
