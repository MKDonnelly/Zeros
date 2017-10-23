
#include "isr.h"

//This function initilizes the
//whole interrupt system. It creates
//and populates interrupt service 
//routines in the IDT and loads the IDT.
void install_isrs(){

   //Create each isr
   add_idt_entry(0, (u32)isr0);
   add_idt_entry(33, (u32)isr33);

   //Load the idt
   load_idt();
}

//Below are the individual interrupt
//service routines. When install_isrs
//is executed, it installs each interrupt
//(number <int#>) into the IDT and sets
//isr<int#> (int interrupt.asm) as the handler.
//each isr<int#> sets up the interrupt, calls
//the c handler (those below), and once done,
//cleans up the interrupt.

//Catches divide by zero faults. Not much we can
//do to correct it (now), so this just halts the machine.
//This is interrupt #0
void zero_divide(){
   k_print("Division by zero detected!");
   __asm__("hlt");
}

//This is called whenever the keyboard generates
//and interrupt through the PIC.
//This is Interrupt #33
void keyboard_handler(){
   k_print("Key pressed");
   __asm__("hlt");
}

