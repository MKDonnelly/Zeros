#include <arch/x86/pmode/isr.h>

#include <arch/x86/cpu.h>
#include <arch/x86/pmode/idt.h>
#include <arch/x86/drivers/vgacommon/vgacommon.h>
#include <arch/x86/drivers/pic.h>
#include <arch/x86/pmode/context.h>
#include <arch/x86/portio.h>
#include <arch/x86/pmode/descriptors.h>
#include <lib/print.h>
#include <lib/bitwise.h>
#include <lib/string.h>

//Bitfield of which interrupts are currently
//registered.
static uint8_t int_present[ TOTAL_INTERRUPTS / 8 ];

//This holds an array of handlers for the interrupts
static void (*int_handlers[TOTAL_INTERRUPTS])();

//This function initilizes the
//whole interrupt system. It creates
//and populates interrupt service 
//routines in the IDT and loads the IDT.
void interrupts_init(){

   //Zero out every interrupt in the
   //interrupt enabled array, just to
   //be safe
   memset( int_present, TOTAL_INTERRUPTS / 8, 0 );

   //This does x = 0..255
   //add_idt_entry(x, (u32)isrx);
   idt_init();
}

//Places the handler function into the interrupt handler array 
//This is exported to general kernel code.
void arch_register_interrupt( uint8_t int_number, void (*handler)()){
  //Mark the interrupt as present
  bit_set( &int_present, int_number );
  int_handlers[int_number] = handler;
}

void arch_unregister_interrupt( uint8_t int_number ){
   //Mark the interrupt as not present
   //and null out the function pointer
   bit_clear( &int_present, int_number);

   //Null out function pointer to handler
   int_handlers[int_number] = 0;
}

//Every interrupt is directed towards this function and is 
//then routed to the correct handler.
void main_interrupt_handler(context_t *r){

   //Check to see if this interrupt came from a PIC.
   //If it did, send the appropriate EOI
   if( r->int_number >= PIC_MASTER_START 
		   && r->int_number <= PIC_MASTER_END ){
      //Interrupt came from master pic
      portb_write( MASTER_PIC_CTRL_P, PIC_EOI_C );
   }else if( r->int_number >= PIC_SLAVE_START 
		   && r->int_number <= PIC_SLAVE_END ){
      portb_write( MASTER_PIC_CTRL_P, PIC_EOI_C );
      portb_write( SLAVE_PIC_CTRL_P, PIC_EOI_C );
   }

   //check to see if there is a registered interrupt
   if( bit_get( &int_present, r->int_number ) ){ 
      //If there is, call the function handler
      int_handlers[ r->int_number ]();
   }else{
      k_printf("Caught unregistered interrupt %d\n", r->int_number);

      //So far, we cannot recover from exceptions
      if( r->int_number < 33 )
         arch_stop_cpu();
   }
}
