#include "isr.h"

//This function initilizes the
//whole interrupt system. It creates
//and populates interrupt service 
//routines in the IDT and loads the IDT.
void install_interrupts(){

   //Zero out every interrupt in the
   //interrupt enabled array, just to
   //be safe
   for(int i = 0; i < TOTAL_INTERRUPTS; i++){
      int_present[i] = 0;
   }

   //Install each isr into the IDT
   add_idt_entry(0, (u32)isr0);
   //Handle the PIC interrupts sent to 
   //IVs 32-40 
   add_idt_entry(30, (u32)isr30);  
   add_idt_entry(31, (u32)isr31);
   add_idt_entry(32, (u32)isr32);  //Timer handler
   add_idt_entry(33, (u32)isr33);  //Keyboard interrupt
   add_idt_entry(34, (u32)isr34);
   add_idt_entry(35, (u32)isr35);
   add_idt_entry(36, (u32)isr36);
   add_idt_entry(37, (u32)isr37);
   add_idt_entry(38, (u32)isr38);
   add_idt_entry(39, (u32)isr39);

   //Load the idt
   load_idt();
}

//Places the handler function into the interrupt handler array 
void register_interrupt( int int_number, void (*handler)(struct registers) ){
  //Mark the interrupt as present
  int_present[int_number] = 1;
  int_handlers[int_number] = handler; 
}


void main_interrupt_handler(struct registers r){

   //Make sure the interrupt has been registered
   if( int_present[ r.int_number ]){ 
      //And call the handler in the interrupt handler
      //array
      int_handlers[ r.int_number ]( r );
   }else{
      k_print("Caught unregistered interrupt: ");
      char number[4];
      itoa( r.int_number, number );
      k_print( number );
      k_newline();
   }
}
