

; Define the global descriptor table
gdt_start:
   gdt_null:   ; the descriptor table must start with 8
     dd 0x0    ; null bytes.
     dd 0x0

   gdt_code:   ; the code segment GDT
      ; base = 0x0, limit = 0xfffff
      ; 1st flags: (segment is present)1 (ring level)00 (descriptor type)1 ->
      ;            0b1001
      ;type flags: (code segment=1)1 (allow low-priv access?)0 
      ;            (readable?)1 (accessed)0 -> 0b1010
      ; 2nd flags: (granulatiry)1 (32-bit?)1 (64-bit?)0 (AVL)0 -> 0b1100
      dw 0xffff    ;Limit (bits 0-15)
      dw 0x0       ;Base (bits 0-15)
      db 0x0       ;Base (bits 16-23)
      db 10011010b ; 1st flags and type flags
      db 11001111b  ; 2nd flags, limit (bits 16-19) (limit is broken up)
      db 0x0       ;Base (bits 24-31)

   gdt_data:
      ;Same as data segment except for type flags. Difference:
      ; type flags: (is code?)0 (expand down?)0 (writable?)1 (accessed)0 ->
      ; 0010b
      dw 0xffff    ;Limit (bits 0 -15)
      dw 0x0       ;Base (bits 0-15)
      db 0x0       ;Base (bits 16-23)
      db 10010010b ;1st flags, type flags
      db 11001111b ;2nd flags, limit (bits 16-19)
      db 0x0       ;base (bits 24-31)

   gdt_end:    ;The reason for putting a label at the end of the
               ;GDT is so we can have the assembler calculate
               ;the size of the GDT for the GDT descriptor (below)

; GDT descriptor
; The descriptor is the address and length of the GDT,
; not the actual GDT.
gdt_descriptor:
   dw gdt_end - gdt_start - 1   ;Length of GDT. Subtract one for true size
   dd gdt_start                 ;Address of GDT start

;Define some handy constants for the GDT segment descriptor offsets, which
; are what segment registers must contain when in protected mode. For example,
; when we set DS = 1x10 in PM, the CPU knows that we mean it to use the
; segment described at offset 0x10 (i.e. 16 bytes) in out GDT, which in our
; case is the DATA segment (0x0 -> NULL; 0x08 -> CODE; 0x10 -> DATA)
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

switchToProtectedMode:
   ;Clear interrupts
   cli

   ;Load the GDT
   lgdt [gdt_descriptor]

   ;Set the bit in cr0
   mov eax, cr0
   or eax, 0x1
   mov cr0, eax

   ;The cpu is now in 32 bit mode.
   ;However, 16 bit instructions are in
   ;the pipeline and must be cleared.
   jmp CODE_SEG:init_protected_mode

[bits 32]

;Initialize the registers and stack in PM
init_protected_mode:
   
   mov ax, DATA_SEG     ;In PM, the old segments are meaningless,
   mov ds, ax           ;so we point our segment registers to the
   mov ss, ax           ;data selector we defined in out GDT
   mov es, ax
   mov fs, ax
   mov gs, ax

   mov ebp, 0x90000     ;Update the stack positor so it is right
   mov esp, ebp         ;at the stop of the free space
   call start_protected_mode
   
