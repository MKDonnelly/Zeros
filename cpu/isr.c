
#include "isr.h"
#include "idt.h"
#include "../drivers/vgatext/print.h"
#include "../lib/string.h"

void install_isrs(){

   //Add a generic interrupt handler for
   //any interrupt not explicitly initilized.
   //for( int i = 0; i < IDT_ENTRIES; i++){
   //   add_idt_entry(i, (u32)generic_int)
   //}
   add_idt_entry(2, (u32)isr2);
   load_idt();
}


void main_int_handler(reg_struct r){
   k_print("received interrupt #");
   char inum[3];
   itoa(r.int_no, inum);
   k_print(inum);
   k_newline();
   __asm__("hlt");
}

