OUTPUT_FORMAT(elf32-i386)
/*ENTRY(arch_start)*/
ENTRY(loader)

SECTIONS{

   . = 0xC0100000;
   .text : AT(ADDR(.text) - 0xC0000000 ) {
      *(.text)
      *(.rodata*)
   }

   .data ALIGN (0x1000) : AT(ADDR(.data) - 0xC0000000) {
       *(.data)
   }

   .bss : AT(ADDR(.bss) - 0xC0000000) {
       _sbss = .;
       *(COMMON)
       *(.bss)
       _ebss = .;
   }


/*   .kernel_text 0xC0100000:
      { test.o }*/
/*   . = 0xC0100000;
   .text.kernel : { test.o }
   . = 0xC0100000;
   .text.kernel : { *(.text) }   
   .data.kernel : { *(.data) }
   .bss.kernel  : { *(.bss) }*/
}
