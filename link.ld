OUTPUT_FORMAT(elf32-i386)
ENTRY(arch_start)
SECTIONS{
   . = 0x100000;  
   .text : {*(.text) }
   .data : {*(.data) }
   .bss : { *(.bss) }
}
