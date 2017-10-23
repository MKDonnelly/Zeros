
#include "pic.h"

//Initilize the PIC
void init_pic(){
  portb_write( 0x20, 0x11 );
  portb_write( 0xA0, 0x11 );

  portb_write( 0x21, 0x20 );
  portb_write( 0xA1, 0x28 );

  portb_write( 0x21, 0x00 );
  portb_write( 0xA1, 0x00 );

  portb_write( 0x21, 0x01 );
  portb_write( 0xA1, 0x01 );

  //Mask all interrupts
  portb_write( 0x21, 0xff );
  portb_write( 0x21, 0xff );
}

//irq1 is the keyboard interrupt
void enable_irq1(){
   //0xfd = 11111101, so irq1 is enabled
   portb_write( 0x21, 0xfd );
}

