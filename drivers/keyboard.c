
#include "keyboard.h"

char KEYBOARD_BUFFER[ KEYBOARD_BUFFER_SIZE ];
int KEYBOARD_BUFFER_CHARS;
//This lets getline do a quick check to see
//if there is a line to return.
int KEYBOARD_NUM_LINES = 0;

void (*return_callback)();
uint8_t return_callback_present = 0;

//Make sure the PIC is remapped before
//calling this.
void init_keyboard(){

   //Enable the keyboard irq
   enable_irq( KEYBOARD_IRQ );

   //And set the interrupt handler
   register_interrupt( KEYBOARD_INTERRUPT, keyboard_handler );

   //Initilize the number of characters in the keyboard
   //buffer
   KEYBOARD_BUFFER_CHARS = 0;
}

void kbd_register_callback( void (*func)() ){
   return_callback = func;
   return_callback_present = 1;
}

//Manages adding and maintaining the keyboard
//buffer as new keys come in
void add_keyboard_buffer(int8_t key){

   if( key == '\r' ){
      KEYBOARD_NUM_LINES++;
   }

   if( KEYBOARD_BUFFER_CHARS < KEYBOARD_BUFFER_SIZE ){
      //The array has space, append the key.
      KEYBOARD_BUFFER[ KEYBOARD_BUFFER_CHARS ] = key;
      KEYBOARD_BUFFER_CHARS++;
   }else{
      //Keyboard buffer does not have space, shift the
      //first element out and all the other elements
      //down. Only go up to KEYBOARD_BUFFER_SIZE - 2 since
      //in an array with 10 elements, 9 is the last valid 
      //index. In addition, when at i = n, the n+1 element is
      //copied over.
      for(int i = 0; i < KEYBOARD_BUFFER_SIZE - 2; i++){
          KEYBOARD_BUFFER[i] = KEYBOARD_BUFFER[i+1];
      }
      KEYBOARD_BUFFER[KEYBOARD_BUFFER_CHARS] = key;
   }
}


//Grabs a line out of the keyboard buffer and removes
//it from the buffer. Ensures that no more than maxlen
//characters are copied over.
//TODO getline cuts off the first character of input
uint8_t getline(int8_t *buffer, int maxlen){

   if( KEYBOARD_NUM_LINES == 0 )
      return 0;

   //Go through the keyboard buffer and copy 
   //over characters to the buffer passed until
   //a \r, null, or \n is hit
   int bufferIndex = 0;
   while( bufferIndex < KEYBOARD_BUFFER_CHARS && 
	  KEYBOARD_BUFFER[bufferIndex] != '\n' &&
          KEYBOARD_BUFFER[bufferIndex] != 0    &&
          KEYBOARD_BUFFER[bufferIndex] != '\r' &&
          bufferIndex < maxlen  ){
      buffer[bufferIndex] = KEYBOARD_BUFFER[bufferIndex];
      bufferIndex++;
   } 
   buffer[bufferIndex] = 0; //Set the NULL bit

   //Move the characters in the buffer down a slot
   //Pre-increment buffer index since it will be pointing
   //to the 0,\n,or \r after the while loop above
   for(int freeSpace = 0, nextChar = ++bufferIndex; nextChar < KEYBOARD_BUFFER_CHARS; freeSpace++, nextChar++){
      KEYBOARD_BUFFER[freeSpace] = KEYBOARD_BUFFER[nextChar];
   }

   //Add 1 since bufferIndex is and index, not a count
   KEYBOARD_BUFFER_CHARS -= bufferIndex+1;

   //We now have 1 less line
   KEYBOARD_NUM_LINES--;

   //Return success
   return 1;
}

//Get a single character from the buffer
//Return 0 if there are no characters to read
char getc(){

   if( KEYBOARD_BUFFER_CHARS == 0 )
      return 0;

   char result = KEYBOARD_BUFFER[0];

   //Shift the characters down in the buffer
   for(int i = 0; i < KEYBOARD_BUFFER_CHARS; i++)
      KEYBOARD_BUFFER[i] = KEYBOARD_BUFFER[i+1];
   
   //We just read a character, so decrement the
   //total number of chars available
   KEYBOARD_BUFFER_CHARS--;
  
   return result; 
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


//This is called whenever the keyboard generates
//and interrupt through the PIC.
//This is Interrupt #33
void keyboard_handler(){

   static uint8_t shift_activated = 0;

   int8_t key;
   uint8_t kb_status = portb_read( KEYBOARD_CONTROLLER_P );

   //Write the key to the screen and place it in
   //the buffer
   if( kb_status & KC_OUTPUTBUF_M ){
      key = portb_read( KEYBOARD_ENCODER_P );
      if( key < 0 )
	      return;

      if( key == SHIFT_RIGHT_KEY || key == SHIFT_LEFT_KEY){
         shift_activated = 1;
      }else if( shift_activated ){
         k_putchar( shift_keycode_to_char[(int)key]);
         add_keyboard_buffer( shift_keycode_to_char[(int)key] );
         shift_activated = 0;
      }else{
         k_putchar( keycode_to_char[(int)key] );
         add_keyboard_buffer( keycode_to_char[(int)key] );
      }
   }
}

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

