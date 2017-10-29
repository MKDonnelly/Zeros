#include "isr.h"

//This function initilizes the
//whole interrupt system. It creates
//and populates interrupt service 
//routines in the IDT and loads the IDT.
void install_interrupts(){

   //Create each isr
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
   asm("hlt");
}

//This is called for unused PIC interrupts
void default_pic(){
   //Make sure to send the EOI bit to
   //re-enable PIC interrupts
   portb_write(0x20, 0x20);
}

//This is called whenever the keyboard generates
//and interrupt through the PIC.
//This is Interrupt #33
void keyboard_handler(){

   ubyte kb_status;
   sbyte key;

   //Write EOI, or else the PIC will not
   //trigger again
   portb_write( 0x20, 0x20 );

   kb_status = portb_read( 0x64 );

   if( kb_status & 0x01 ){
      key = portb_read( 0x60 );
      if( key < 0 )
	      return;
      k_putchar( keyboard_map[(int)key] );
   }

}

udword ticks = 0;

void timer_handler(){
   ticks++;
   k_print("Tick: ");

   char ta[100];
   itoa( ticks, ta );
   k_print( ta );
   k_newline();

   //Write EOI, or else the timer
   //will not trigger again
   portb_write( 0x20, 0x20 );
}

void main_interrupt_handler(struct registers r){

   switch( r.int_number ){
      case 0:
	      zero_divide();
	      break;
      case 33:
	      keyboard_handler();
	      break;
      default:
              k_print("Interrupt called: #");
	      char s[4];
	      itoa( r.int_number, s);
	      k_print(s);
	      k_newline();
	      break;
   }
}
