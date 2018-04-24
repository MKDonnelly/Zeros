#include <arch/x86/pmode/idt.h>

//The interrupt descriptor as well as the interrupt able
static idt_entry_t idt_table[ TOTAL_INTERRUPTS ];
static idt_descriptor_t idt_descriptor;

void add_idt_entry( uint8_t intNumber, uint32_t handlerFunction) {

   //IDT_TABLE is the interrupt descriptor table
   idt_table[intNumber].lower_offset = handlerFunction & 0xFFFF;
   idt_table[intNumber].segment_sel = gdt_kernel_code;  
   idt_table[intNumber].zeroed = 0;

   //This is a hack for system call interrupt
   if( intNumber == 0x31 )
      idt_table[intNumber].flags = 0b11101110;
   else
      idt_table[intNumber].flags = 0x8E; //1 00 0 1110, see idt.h
   idt_table[intNumber].higher_offset = (handlerFunction >> 16) & 0xFFFF;
}

void load_idt(){
   //Load the base and length of the idt into the idt descriptor
   idt_descriptor.base_addr = (uint32_t) &idt_table;
   //the actual size is len - 1
   idt_descriptor.length = TOTAL_INTERRUPTS * sizeof(idt_entry_t) - 1;

   arch_disable_ints();
   arch_load_idt( &idt_descriptor );
}
