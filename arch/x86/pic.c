
#include "pic.h"


//Initilize the PIC
void remap_pic(){

  //Restart each PIC for configuration
  //Configure each pic to use ICW4
  portb_write( MASTER_PIC_CTRL_P, PIC_ICW1_INIT_C | PIC_ICW1_ENABLE_ICW4_C );
  portb_write( SLAVE_PIC_CTRL_P, PIC_ICW1_INIT_C | PIC_ICW1_ENABLE_ICW4_C );

  //Master pic will have interrupts starting
  //at 32 and slave pic will have interrupts
  //starting at 40.
  portb_write( MASTER_PIC_DATA_P, PIC_MASTER_START );
  portb_write( SLAVE_PIC_DATA_P, PIC_SLAVE_START );

  //Configure each pic for cascading
  portb_write( MASTER_PIC_DATA_P, 4 ); //Slave PIC on IRQ2 (b0100)
  portb_write( SLAVE_PIC_DATA_P, 2 );  //Slave PIC cascade identity

  //Configure ICW 4
  portb_write( MASTER_PIC_DATA_P, PIC_ICW4_8086_MODE_C );
  portb_write( SLAVE_PIC_DATA_P, PIC_ICW4_8086_MODE_C );

  //Mask all interrupts by setting every bit to 1
  portb_write( MASTER_PIC_DATA_P, 0xff );
  portb_write( SLAVE_PIC_DATA_P, 0xff );
}

void enable_irq(uint8_t num){
   int8_t curPICMask = portb_read( MASTER_PIC_DATA_P );
   int8_t mask = ~(0x1 << num);
   portb_write( MASTER_PIC_DATA_P, curPICMask & mask );
}

void disable_irq(uint8_t num){
   int8_t curPICMask = portb_read( MASTER_PIC_DATA_P );
   int8_t mask = 0x1 << num;
   portb_write( MASTER_PIC_DATA_P, curPICMask | mask);
}
