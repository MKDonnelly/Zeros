
#include "pic.h"


//Initilize the PIC
void remap_pic(){

  //Restart each PIC for configuration
  //Configure each pic to use ICW4
  portb_write( MASTER_PIC_CTRL, PIC_ICW1_INIT | PIC_ICW1_ENABLE_ICW4 );
  portb_write( SLAVE_PIC_CTRL, PIC_ICW1_INIT | PIC_ICW1_ENABLE_ICW4 );

  //Master pic will have interrupts starting
  //at 32 and slave pic will have interrupts
  //starting at 40.
  portb_write( MASTER_PIC_DATA, PIC_MASTER_START );
  portb_write( SLAVE_PIC_DATA, PIC_SLAVE_START );

  //Configure each pic for cascading
  portb_write( MASTER_PIC_DATA, 4 ); //Slave PIC on IRQ2 (b0100)
  portb_write( SLAVE_PIC_DATA, 2 );  //Slave PIC cascade identity

  //Configure ICW 4
  portb_write( MASTER_PIC_DATA, PIC_ICW4_8086_MODE );
  portb_write( SLAVE_PIC_DATA, PIC_ICW4_8086_MODE );

  //Mask all interrupts
  portb_write( MASTER_PIC_DATA, 0xff );
  portb_write( SLAVE_PIC_DATA, 0xff );
}

/*
//Initilize the PIC
void remap_pic(){
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
}*/


