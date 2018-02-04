#include <arch/x86/idt.h>

void add_idt_entry( uint8_t intNumber, uint32_t handlerFunction) {

   //IDT_TABLE is the interrupt descriptor table
   //IDT_TABLE[intNumber].lower_offset = lower_16(handlerFunction);
   IDT_TABLE[intNumber].lower_offset = handlerFunction & 0xFFFF;
   IDT_TABLE[intNumber].segment_sel = gdt_kernel_code;  
   IDT_TABLE[intNumber].zeroed = 0;
   IDT_TABLE[intNumber].flags = 0x8E; //1 00 0 1110, see idt.h
   //IDT_TABLE[intNumber].higher_offset = upper_16(handlerFunction);
   IDT_TABLE[intNumber].higher_offset = (handlerFunction >> 16) & 0xFFFF;
}

void load_idt(){
   //idt_des is the interrupt table descriptor
   IDT_DESCRIPTOR.base_addr = (uint32_t) &IDT_TABLE;
   IDT_DESCRIPTOR.length = TOTAL_INTERRUPTS * sizeof(idt_entry_t) - 1;

   asm volatile("cli");
   asm volatile("lidtl (%0)" : : "r" (&IDT_DESCRIPTOR) );
}
