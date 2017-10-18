
#include "isr.h"
#include "idt.h"
#include "../drivers/vgatext/print.h"
#include "../lib/string.h"

void isr_install(){

   set_idt_gate(2, (u32)isr2);
   set_idt();
}

void isr_handler(reg_struct r){
   k_print("received interrupt#");
   char inum[3];
   itoa(r.int_no, inum);
   k_print(inum);
   k_newline();
}

