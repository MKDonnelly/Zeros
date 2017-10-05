; OS assembly library

NEWLINE equ 10

; Print a number as hex on the screen
; dx = number to convert
print_hex:
   pusha
   mov si, .HEX_OUT + 2

 .next_character:
   mov bx, dx
   and bx, 0xf000
   shr bx, 4
   add bh, 0x30
   cmp bh, 0x39
   jg .add_7

 .add_character_hex:
   mov al, bh
   mov [si], bh
   inc si
   shl dx, 4
   or dx, dx
   jnz .next_character
   mov bx, .HEX_OUT
   call print_string
   popa
   ret
 .add_7:
   add bh, 0x7
   jmp .add_character_hex
 .HEX_OUT:
   db '0x0000', 0

; Prints the string at bx
; to the screen using the bios
print_string:
   pusha
   mov ah, 0xe
 .print_character:
   mov al, [bx]
   inc bx
   or al, al
   jz .print_done
   int 0x10
   jmp .print_character
 .print_done:
   popa
   ret
