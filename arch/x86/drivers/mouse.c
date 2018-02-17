#include <arch/x86/drivers/mouse.h>

int8_t mouse_buffer[3];
int offset = 0;

void mouse_wait(int a_type) //unsigned char
{
  int _time_out=100000; //unsigned int
  if(a_type==0)
  {
    while(_time_out--) //Data
    {
      if((portb_read(0x64) & 1)==1)
      {
        return;
      }
    }
    return;
  }
  else
  {
    while(_time_out--) //Signal
    {
      if((portb_read(0x64) & 2)==0)
      {
        return;
      }
    }
    return;
  }
}

void mouse_write(int8_t a_write) //unsigned char
{
  //Wait to be able to send a command
  mouse_wait(1);
  //Tell the mouse we are sending a command
  portb_write(0x64, 0xD4);
  //Wait for the final part
  mouse_wait(1);
  //Finally write
  portb_write(0x60, a_write);
}

int8_t mouse_read()
{
  //Get's response from mouse
  mouse_wait(0);
  return portb_read(0x60);
}

void mouse_init(){ 

   //Enable aux mouse
   mouse_wait(1);
   portb_write( 0x64, 0xA8 );

   //Enable interrupt
   /////////////////////////////////////
   mouse_wait(1);
   //Get ready to read the status bit
   portb_write( 0x64, 0x20 ); 

   mouse_wait(0);
   //Set bit 2 and clear bit 5 to enable mouse IRQ 12
   uint8_t status_bit = portb_read( 0x60 ) | 2;

   mouse_wait(1);
   //Send status byte back
   portb_write( 0x64, 0x60 ); //set bit
   mouse_wait(1);
   portb_write( 0x60, status_bit ); 
//   mouse_wait(0);
//   portb_read(0x60);
   //////status bit has been set
  
   //Read back status
   portb_write( MOUSE_CMD_P, 0x20 );
   int8_t data = portb_read( MOUSE_DATA_P );
   k_printf("Mouse config: %x\n", data); 

   //Tell mouse to use defaults
   mouse_write(0xF6);
   mouse_read(); //ack

   //enable mouse
   mouse_write(0xF4);
   mouse_read();//ack

   //Enable interrupt and setup handler
   //enable_irq( MOUSE_ISR );
   register_interrupt( MOUSE_INTERRUPT, mouse_interrupt_handler );
}

void mouse_interrupt_handler(){

   k_printf("Mouse handler\n");
   while(1);
/*
   int8_t status = portb_read( MOUSE_CMD_P );

   //There is only data for the mouse if bitmask 0x20 is set
   if( ! (status & 0x20 ) )
      return;

   //mouse returns data in 3 separate packets (in separate interrupts)
   mouse_buffer[ offset ] = portb_read( MOUSE_DATA_P );
   offset = (offset + 1) % 3;

   k_printf("Mouse: %d %d %d\n", mouse_buffer[0], mouse_buffer[1], mouse_buffer[2] );
*/
}

