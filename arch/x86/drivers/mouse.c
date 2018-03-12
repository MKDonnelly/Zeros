#include <arch/x86/drivers/mouse.h>

//Temporarily holds packets as they come in.
//When a full packet has been accuumulated, it
//is converted to a mouse_packet_t to the registered
//handler.
int8_t mouse_buffer[3];

//Which packet are we at? Since the packets
//come in along three separate interrupts, we
//need to determine which packet we are at.
int offset = 0;

//Higher level code sets this callback using 
//register_mouse_handler
void (*mouse_handler)(mouse_packet_t) = NULL;

//We must wait for the mouse to respond before
//writing to port 0x60 or 0x64.
void wait_for_write(){
   for(int i = 0; i < MOUSE_TIMEOUT; i++){
      if( (portb_read( MOUSE_CMD_P ) & MOUSE_WRITE_OK_M) == 0 )
         return;
   }
}

//We must wait for the mouse to respond before
//reading from reading from port 0x60
void wait_for_read(){
   for(int i = 0; i < MOUSE_TIMEOUT; i++){
      if( (portb_read( MOUSE_CMD_P ) & MOUSE_READ_OK_M) == 1 )
         return;
   }
}

//Write a command to the 
void mouse_write(int8_t command){

  //Wait to be able to send a command
  wait_for_write();

  //Tell the mouse we are sending a command
  portb_write(MOUSE_CMD_P, MOUSE_WRITE_COMING_C);

  //Wait for the final part
  wait_for_write();

  //Finally write
  portb_write(MOUSE_DATA_P, command);
}


//Used to conllect mouse packets and pass them
//up to whatever handler has been defined.
void mouse_interrupt_handler(){

   int8_t status = portb_read( MOUSE_CMD_P );

   //There is only data for the mouse if bitmask 0x20 is set
   if( ! (status & 0x20 ) )
      return;

   //mouse returns data in 3 separate packets (in separate interrupts)
   mouse_buffer[ offset ] = portb_read( MOUSE_DATA_P );
   offset = (offset + 1) % 3;

   //Call higher level handler if the packet is complete
   //and the mouse handler is present.
   if( offset == 0 && mouse_handler != NULL){
      //convert the raw packets into a mouse_packet_t, for
      //use by higher-level code
      mouse_packet_t packet = (mouse_packet_t){
         .left_button = mouse_buffer[0] & MOUSE_LEFT_BUTTON,
         .middle_button = mouse_buffer[0] & MOUSE_MIDDLE_BUTTON,
         .right_button = mouse_buffer[0] & MOUSE_LEFT_BUTTON,
         .delta_x = mouse_buffer[1],
         .delta_y = mouse_buffer[2]
      };
      mouse_handler( packet );
   }
}


void mouse_init(){ 

   //Enable aux mouse
   wait_for_write();
   portb_write( MOUSE_CMD_P, MOUSE_AUX_MOUSE );

   //Enable interrupt by setting bit 2 of 
   //the status byte
   wait_for_read();

   //Get ready to read the status byte
   portb_write( MOUSE_CMD_P, 0x20 ); 

   wait_for_read();
   //Set bit 2 to enable mouse IRQ 12
   uint8_t status_bit = portb_read( MOUSE_DATA_P ) | 0x2;

   wait_for_write();
   //Send status byte back
   portb_write( MOUSE_CMD_P, MOUSE_SEND_COMPAQ_C ); //set bit
   wait_for_write();
   portb_write( MOUSE_DATA_P, status_bit ); 
  
   //Tell mouse to use defaults
   mouse_write(MOUSE_SET_DEFAULTS_C);
   //ack mouse
   wait_for_read();
   (void)portb_read(MOUSE_DATA_P);

   //enable mouse
   mouse_write(MOUSE_ENABLE_PSTREAM);
   //ack mouse
   wait_for_read();
   (void)portb_read(MOUSE_DATA_P);

   //Enable interrupt and setup handler
   enable_irq( MOUSE_ISR );
   arch_register_interrupt( MOUSE_INTERRUPT, mouse_interrupt_handler );
}

//Pass null to unregister handler
void register_mouse_handler( void (*new_handler)(mouse_packet_) ){
   mouse_handler = new_handler;
}

