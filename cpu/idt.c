
#include "idt.h"
#include "types.h"

void add_idt_entry( int intNumber, u32 handlerFunction) {
   //int_table is the interrupt descriptor table
   int_table[intNumber].lower_offset = lower_16(handlerFunction);
   int_table[intNumber].segment_sel = KERNEL_CS;  //KERNEL_CS defined in idt.h
   int_table[intNumber].zeroed = 0;
   int_table[intNumber].flags = 0x8E; //1 00 0 1110, see idt.h
   int_table[intNumber].higher_offset = upper_16(handlerFunction);
}

void load_idt(){
   //idt_des is the interrupt table descriptor
   idt_des.base_addr = (u32) &int_table;
   idt_des.length = IDT_ENTRIES * sizeof(idt_entry) - 1;
   __asm__ __volatile__("lidtl (%0)" : : "r" (&idt_des) );
}
