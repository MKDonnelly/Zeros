[org 0x7c00]

KERNEL_OFFSET equ 0x1000

mov bp, 0x8000
mov sp, bp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Switch to 13h video mode
;mov ax, 0x13
;int 0x10
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;Load kernel sectors
mov ah, 0x02
mov al, 50    ;Load this many 512-bit sectors
              ;Set it high just to be sure that
              ;as the kernel gets larger, everything
              ;is loaded.
;dl does not need to be changed
mov ch, 0
mov dh, 0
mov cl, 2
mov bx, KERNEL_OFFSET
int 0x13

call switch_to_PM  ;This never returns


%include "boot/realMode.asm"
%include "boot/32switch.asm"
%include "boot/protectedMode.asm"

[bits 32]

start_protected_mode:
   call KERNEL_OFFSET
   jmp $


; Padding and magic number
times 510-($-$$) db 0
dw 0xaa55

