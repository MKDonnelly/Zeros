OUTPUT_FORMAT(elf32-i386)
ENTRY(loader)

SECTIONS{

   /*This sets the linker's Virtual Memory address*/
   . = 0xC0100000;
   ldscript_text_start = .;
   /*When using AT, we are actually setting the linker's
     Load Memory Address, for use by GRUB when loading*/
   .text : AT(ADDR(.text) - 0xC0000000 ) {
      *(.text)
   }
   ldscript_text_end = .;

   ldscript_data_start = .;
   .data ALIGN (0x1000) : AT(ADDR(.data) - 0xC0000000) {
       *(.data)
   }
   ldscript_data_end = .;

   ldscript_bss_start = .;
   .bss : AT(ADDR(.bss) - 0xC0000000) {
       *(.bss)
   }
   ldscript_bss_end = .;
   ldscript_kernel_end = .;
}
