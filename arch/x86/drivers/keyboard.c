#include <arch/x86/drivers/keyboard.h>

#include <arch/x86/pmode/isr.h>
#include <arch/x86/portio.h>
#include <arch/x86/drivers/pic.h>

void (*keyboard_callback)(char) = 0;

//Thin layer to transform the input scancode
//into a char and send it to the callback
static void keyboard_handler(){

   static uint8_t shift_activated = 0;

   int8_t key;
   uint8_t kb_status = portb_read( KEYBOARD_CONTROLLER_P );

   if( kb_status & KC_OUTPUTBUF_M ){
      key = portb_read( KEYBOARD_ENCODER_P );
      if( key < 0 )
	      return;

      if( key == SHIFT_RIGHT_KEY || key == SHIFT_LEFT_KEY){
         shift_activated = 1;
      }else if( shift_activated ){
         keyboard_callback( shift_keycode_to_char[(int)key]);
         shift_activated = 0;
      }else{
         keyboard_callback( keycode_to_char[(int)key] );
      }
   }
}


//Make sure the PIC is remapped before
//calling this. keypress_callback is called
//whenever a key is pressed. The argument is
//the key character entered.
void arch_keyboard_init( void (*keypress_callback)(char) ){

   //Enable the keyboard irq
   enable_irq( KEYBOARD_IRQ );

   //And set the interrupt handler
   arch_register_interrupt( KEYBOARD_INTERRUPT, keyboard_handler );

   keyboard_callback = keypress_callback;
}

//Register a different callback if needed
void arch_keyboard_register_callback( void (*keypress_callback)(char) ){
   keyboard_callback = keypress_callback;
}


//               Scan Code Set 1
// '\0' = Not implemented
// Ascii DC1 = Arrow up
// Ascii DC2 = Arrow Down
// Ascii DC3 = Arrow left
// Ascii DC4 = Arrow Right
// Insert = Start of Heading
// Home = Start Of Text
// Page Up = End Of Text
// End = End Of Transmission
// Page Down = Enquiry
// CTRL = Acknowledge
// Alt = Shift Out
int8_t keycode_to_char[256] = {
   //Keys 0-9
   '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8',
   //Keys 10-19
   '9', '0', '-', '=', '\b', '\0', 'q', 'w', 'e', 'r',
   //Keys 20-29
   't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\r', 14,
   //Keys 30-39
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
   //Keys 40-49
   '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n',
   //Keys 50-59
   'm', ',', '.', '/', '\0', '\0', '\0', ' ', '\0', '\0',
   //Keys 60-69
   '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
   //Keys 70-79
   '\0', 2, 17, 3, '\0', 13, '\0', 14, '\0', 4, 
   //Keys 80-89
   12, 5, 1, 127, '\0', '\0', '\0', '\0', '\0', '\0', 
   //Keys 90-99
   6, '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
};

int8_t shift_keycode_to_char[256] = {
   //Keys 0-9
   '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*',
   //Keys 10-19
   '(', ')', '_', '+', '\b', '\0', 'Q', 'W', 'E', 'R',
   //Keys 20-29
   'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\r', 14,
   //Keys 30-39
   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
   //Keys 40-49
   '"', '~', '\0', '|', 'Z', 'X', 'C', 'V', 'B', 'N',
   //Keys 50-59
   'M', '<', '>', '?', '\0', '\0', '\0', ' ', '\0', '\0',
   //Keys 60-69
   '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
   //Keys 70-79
   '\0', 2, 17, 3, '\0', 13, '\0', 14, '\0', 4, 
   //Keys 80-89
   12, 5, 1, 127, '\0', '\0', '\0', '\0', '\0', '\0', 
   //Keys 90-99
   6, '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
};


//Send a command to the keyboard controller
void kbd_ctrl_send_command(uint8_t command){
   //Spin while the keyboard is busy
   while( (portb_read( KEYBOARD_CONTROLLER_P ) & 
          KC_INPUTBUF_M ) != 0);

   //Now that the keyboard is free, send 
   //the command
   portb_write( KEYBOARD_CONTROLLER_P, command );
}

//Send a command to the keyboard encoder 
void kbd_enc_send_command(uint8_t command){
   while( (portb_read( KEYBOARD_CONTROLLER_P ) &
           KC_INPUTBUF_M) != 0);
   portb_write( KEYBOARD_ENCODER_P, command);
}


//Sets the LEDs on the keyboard
void kb_set_leds(uint8_t numLock, uint8_t capLock, uint8_t scroll){
   uint8_t leds = 0;
   if( numLock )
      leds |= KE_NUMLOCK_M;
   if( capLock )
      leds |= KE_CAPLOCK_M;
   if( scroll )
      leds |= KE_SCROLL_M;

   //Send the command to the encoder
   kbd_enc_send_command( KE_SET_LEDS_C );
   kbd_enc_send_command( leds );
}

