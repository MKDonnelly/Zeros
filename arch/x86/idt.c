#include <arch/x86/idt.h>

void add_idt_entry( uint8_t intNumber, uint32_t handlerFunction) {

   //IDT_TABLE is the interrupt descriptor table
   IDT_TABLE[intNumber].lower_offset = handlerFunction & 0xFFFF;
   IDT_TABLE[intNumber].segment_sel = gdt_kernel_code;  
   IDT_TABLE[intNumber].zeroed = 0;
   IDT_TABLE[intNumber].flags = 0x8E; //1 00 0 1110, see idt.h
   IDT_TABLE[intNumber].higher_offset = (handlerFunction >> 16) & 0xFFFF;
}

void load_idt(){
   //Load the base and length of the idt into the idt descriptor
   IDT_DESCRIPTOR.base_addr = (uint32_t) &IDT_TABLE;
   //the actual size is len - 1
   IDT_DESCRIPTOR.length = TOTAL_INTERRUPTS * sizeof(idt_entry_t) - 1;

   arch_disable_ints();
   asm volatile("lidtl (%0)" : : "r" (&IDT_DESCRIPTOR) );
}
