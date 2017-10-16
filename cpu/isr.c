
#include "isr.h"
#include "idt.h"
#include "../drivers/print.h"

void isr_install(){

   set_idt_gate(2, (u32)isr2);
   set_idt();
}

void isr_handler(registers_t r){
   k_print("received an interrupt!");
}

