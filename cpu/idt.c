#include "idt.h"

void add_idt_entry( u8 intNumber, u32 handlerFunction) {

   //IDT_TABLE is the interrupt descriptor table
   IDT_TABLE[intNumber].lower_offset = lower_16(handlerFunction);
   IDT_TABLE[intNumber].segment_sel = KERNEL_CS;  //KERNEL_CS defined in idt.h
   IDT_TABLE[intNumber].zeroed = 0;
   IDT_TABLE[intNumber].flags = 0x8E; //1 00 0 1110, see idt.h
   IDT_TABLE[intNumber].higher_offset = upper_16(handlerFunction);
}

void load_idt(){
   //idt_des is the interrupt table descriptor
   IDT_DESCRIPTOR.base_addr = (u32) &IDT_TABLE;
   IDT_DESCRIPTOR.length = TOTAL_INTERRUPTS * sizeof(idt_entry) - 1;

   asm volatile("cli");
   asm volatile("lidtl (%0)" : : "r" (&IDT_DESCRIPTOR) );
}
