OUTPUT_FORMAT(elf64-x86-64)
ENTRY(arch_start)

SECTIONS{
   . = 0x100000;
   .text : { *(.text) }
}
