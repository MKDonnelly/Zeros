
#include "idt.h"
#include "types.h"

void set_idt_gate( int n, u32 handler) {
   //int_table is the interrupt descriptor table
   int_table[n].lower_offset = lower_16(handler);
   int_table[n].segment_sel = KERNEL_CS;  //KERNEL_CS defined in idt.h
   int_table[n].zeroed = 0;
   int_table[n].flags = 0x8E; //1 00 0 1110, see idt.h
   int_table[n].higher_offset = upper_16(handler);
}

void set_idt(){
   idt_des.base_addr = (u32) &int_table;
   idt_des.length = IDT_ENTRIES * sizeof(idt_entry) - 1;
   __asm__ __volatile__("lidtl (%0)" : : "r" (&idt_des) );
}
