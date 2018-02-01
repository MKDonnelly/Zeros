#include "isr.h"
#include <sched.h>

//This function initilizes the
//whole interrupt system. It creates
//and populates interrupt service 
//routines in the IDT and loads the IDT.
void init_interrupts(){

   //Zero out every interrupt in the
   //interrupt enabled array, just to
   //be safe
   for(int i = 0; i < TOTAL_INTERRUPTS; i++){
      bitClear( &int_present, i);
   }

   //This does x = 0..255
   //add_idt_entry(x, (u32)isrx);
   init_idt();
   
   //Load the idt
   load_idt();
}


//Places the handler function into the interrupt handler array 
void register_interrupt( uint8_t int_number, void (*handler)(registers_t)){
  //Mark the interrupt as present
  bitSet( &int_present, int_number );
  int_handlers[int_number] = handler;
}

void unregister_interrupt( uint8_t int_number ){
   //Mark the interrupt as not present
   //and null out the function pointer
   bitClear( &int_present, int_number);
   int_handlers[int_number] = 0;
}

void main_interrupt_handler(registers_t r){

   //Check to see if this interrupt came from a PIC.
   //If it did, send the appropriate EOI
   if( r.int_number >= PIC_MASTER_START 
		   && r.int_number <= PIC_MASTER_START + 8 ){
      //Interrupt came from master pic
      portb_write( MASTER_PIC_CTRL_P, PIC_EOI_C );
   }else if( r.int_number >= PIC_SLAVE_START 
		   && r.int_number <= PIC_SLAVE_START + 8 ){
      portb_write( MASTER_PIC_CTRL_P, PIC_EOI_C );
      portb_write( SLAVE_PIC_CTRL_P, PIC_EOI_C );
   }

   //check to see if there is a registered interrupt
   if( bitGet( &int_present, r.int_number ) ){ 
      //If there is, call the function handler
      int_handlers[ r.int_number ]( r );
   }else{
      k_printf("Caught unregistered interrupt %d\n", r.int_number);

      //Make sure to halt if this is a processor error
      if( r.int_number < 33 )
         asm("hlt");
   }
}
