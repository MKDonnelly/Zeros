
#include "keyboard.h"

//Make sure the PIC is remapped before
//calling this.
void init_keyboard(){

   //Read the current value for the PIC 
   //interrupt mask and set the keyboard
   //bit

   //ubyte curPICMask = portb_read( 0x21 );
   //portb_write( 0x21, curPICMask | 0x2 );      //Second bit is for keyboard

   register_interrupt( 33, keyboard_handler );
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
