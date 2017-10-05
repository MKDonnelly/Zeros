

; Convert the given byte into an
; ascii string representation 
; cl is byte to convert
; DOES NOT WORK VERY WELL
byteToHex:
 mov bl, cl
 shr bl, 4 

.test1Num:
 cmp bl, 9
 jg .test1Let
 call _pHexNum 
 jmp .test2Num
.test1Let:
 call _pHexLet

.test2Num:
 mov bl, cl
 and bl, 0b00001111
 
 cmp bl, 9
 jg .test2Let
 call _pHexNum
 jmp .end

.test2Let:
 call _pHexLet

.end:
  ret


; Convert the hex digits
; 0..9 into '0'..'9' 
; bl is number
_pHexNum:
   cmp bl, 9
   add bl, 48
   mov al, bl
   int 0x10
   ret

; Convert the hex digits
; a..f into 'a'..'f'
; bl is number
_pHexLet:
  add bl, 87
  mov al, bl
  int 0x10
  ret


; Print string routine
; bx is index of string to print
; string must be null-terminated.
printString:
 .mainLoop:
   cmp byte [bx], 0
   je .endLoop   
   mov al, byte [bx]
   int 0x10
   inc bx
   jmp .mainLoop
 .endLoop:
   ret

; NOT WORKING AS A FUNCTION
; Loads sectors from the disk 
; to the designated location
; This accepts the following params:
; al = Number of sectors to read
; dl = boot drive number
; bx = address to place sectors in memory
;diskLoad:
;   ;Save the following registers
;   push ax   ;for ah bios service in use
;
;   mov ah, 0x02   ;BIOS read disk interrupt
;   ;al = number of sectors to read
;   ;dl = boot drive number
;   mov ch, 0      ;cylinder number
;   mov dh, 0      ;head number
;   mov cl, 2      ;starting sector number (#1 = Boot sector)
;   ;bx = location to place sectors
;   int 0x13

;   pop ax
;   ret
