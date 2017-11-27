
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

  //Mask all interrupts by setting every bit to 1
  portb_write( MASTER_PIC_DATA, 0xff );
  portb_write( SLAVE_PIC_DATA, 0xff );
}

void enable_irq(uint8_t num){
   int8_t curPICMask = portb_read( MASTER_PIC_DATA );
   int8_t mask = ~(0x1 << num);
   portb_write( MASTER_PIC_DATA, curPICMask & mask );
}

void disable_irq(uint8_t num){
   int8_t curPICMask = portb_read( MASTER_PIC_DATA );
   int8_t mask = 0x1 << num;
   portb_write( MASTER_PIC_DATA, curPICMask | mask);
}
