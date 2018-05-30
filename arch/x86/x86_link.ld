OUTPUT_FORMAT(elf32-i386)
ENTRY(arch_start)

SECTIONS{

   /*Grub jumps to the virtual address of the ENTRY,
     so we must have it reflect the physical location*/
   . = 0x100000;
   .boot : { *(.boot) }
   boot_end = .;

   /*This sets the linker's Virtual Memory address*/
   . += 0xC0100000; 
   ldscript_text_start = .;
   /*While we are linking somwhere above 0xC0000000, we want
     the text to be placed in lower memory*/
   .text  ALIGN(0x1000) : AT(ADDR(.text) - 0xC0000000 ) {
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

   /*It is easier to include the initrd as part of the
     image, rather than having grub place it wherever
     and then trying to map it in*/
   ldscript_initrd_start = .;
   .initrd : AT(ADDR(.initrd) - 0xC0000000) { 
       *(.initrd)
   }
   ldscript_initrd_end = .;

   ldscript_kernel_end = .;
}
