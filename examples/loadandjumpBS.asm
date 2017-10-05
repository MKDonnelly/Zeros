;                Setup
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[org 0x7c00] ;BIOS loads this program to 0x7c00
             ;all variables are referenced 
             ;relative to this point
;Set bios to output to tty
mov ah, 0x0e

;Initilize stack
mov bp, 0x8000
mov sp, bp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

firstStageBoot:
   ;Load sectors from disk 
   mov byte [boot_drive], dl
   
   mov al, 1                   ;number of sectors to read
   mov dl, [boot_drive]        ;drive to read from
   mov bx, secondStageBoot     ;place read sectors here
   mov ah, 0x02                ;bios disk read mode
   mov ch, 0                   ;cylinder number
   mov dh, 0                   ;head number
   mov cl, 2                   ;starting sector number
   int 0x13                    ;generate interrupt

   jmp secondStageBoot


%include "lib.asm"
boot_drive db 0

;             Padding and magic bit
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
times 510-($-$$) db 0
dw 0xaa55


;Everything below is loaded by the first stage
secondStageBoot:
   mov bx, myMsg
   call print_string
   call sysSuspend


myMsg db "Successfully jumped!", 0
