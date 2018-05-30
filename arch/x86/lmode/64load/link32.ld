OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS{
   . = 0x100000;
   .text : { *(.text) }

   /*The 64 bit image will be cated onto the end*/
   ldscript_kernel64 = .;
}
