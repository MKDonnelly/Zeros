OUTPUT_FORMAT(elf64-x86-64)
ENTRY(arch_start)
SECTIONS{
   . = 0x100000;  
   .text : {*(.text) }
   .data : {*(.data) }
   .bss : { *(.bss) }

   /*Early kernel stack*/
   . += 0x1000;
   ldscript_kernel_end = .;
}
