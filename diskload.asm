; Loads sectors from the disk 
; to the designated location
; This accepts the following params:
; al = Number of sectors to read
; dl = boot drive number
; bx = address to place sectors in memory
diskLoad:
   ;Save the following registers
   push ax   ;for ah bios service in use

   mov ah, 0x02   ;BIOS read disk interrupt
   ;al = number of sectors to read
   ;dl = boot drive number
   mov ch, 0      ;cylinder number
   mov dh, 0      ;head number
   mov cl, 2      ;starting sector number (#1 = Boot sector)
   ;bx = location to place sectors
   int 0x13

   pop ax
   ret
