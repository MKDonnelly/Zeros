[org 0x7c00]

mov bp, 0x9000
mov sp, bp

mov bx, MSG_REAL_MODE
call print_string

call switchToProtectedMode  ;This never returns

jmp $   ;and this never executes

%include "libreal.asm"
%include "32switch.asm"
%include "libpm.asm"

[bits 32]

start_protected_mode:
   mov ebx, MSG_PROT_MODE
   call print_string_pm
   jmp $

;Global variables
MSG_REAL_MODE db "Started in 16-bit Real mode", 0
MSG_PROT_MODE db "Successfully landed in 32-bit Protected Mode", 0

; Padding and magic number
times 510-($-$$) db 0
dw 0xaa55
