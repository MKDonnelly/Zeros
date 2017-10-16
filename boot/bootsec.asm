[org 0x7c00]

KERNEL_OFFSET equ 0x1000

mov bp, 0x9000
mov sp, bp

mov bx, MSG_REAL_MODE
call print_string

;Load kernel sectors
mov ah, 0x02
mov al, 15
;dl does not need to be changed
mov ch, 0
mov dh, 0
mov cl, 2
mov bx, KERNEL_OFFSET
int 0x13

call switch_to_PM  ;This never returns

jmp $   ;and this never executes

%include "./boot/libreal.asm"
%include "./boot/32switch.asm"
%include "./boot/libpm.asm"

[bits 32]

start_protected_mode:
   mov ebx, MSG_PROT_MODE
   call print_string_pm
   call KERNEL_OFFSET
   jmp $

;Global variables
MSG_REAL_MODE db "Started in 16-bit Real mode", 0
MSG_PROT_MODE db "Successfully landed in 32-bit Protected Mode", 0

; Padding and magic number
times 510-($-$$) db 0
dw 0xaa55
