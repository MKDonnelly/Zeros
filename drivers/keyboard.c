
#include "keyboard.h"

char KEYBOARD_BUFFER[ KEYBOARD_BUFFER_SIZE ];
int KEYBOARD_BUFFER_CHARS;

//Make sure the PIC is remapped before
//calling this.
void init_keyboard(){

   //Read the current value for the PIC 
   //interrupt mask and set the keyboard
   //bit
   ubyte curPICMask = portb_read( 0x21 );

   //Set bit 1 to zero so that the keyboard is enabled
   portb_write( 0x21, curPICMask & KEYBOARD_PIC_MASK );

   register_interrupt( 33, keyboard_handler );

   //Initilize the number of characters in the keyboard
   //buffer
   KEYBOARD_BUFFER_CHARS = 0;
}

//Manages adding and maintaining the keyboard
//buffer as new keys come in
void add_keyboard_buffer(char key){

   //The array has space, append the key.
   if( KEYBOARD_BUFFER_CHARS < KEYBOARD_BUFFER_SIZE ){
      KEYBOARD_BUFFER[ KEYBOARD_BUFFER_CHARS ] = key;
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


//Gets a line from the keyboard buffer and deletes it
//from the buffer.
void getline(char *buffer){
   int bufferIndex = 0;
   while( bufferIndex < KEYBOARD_BUFFER_SIZE && \
		   KEYBOARD_BUFFER[bufferIndex] != '\n' ){
      buffer[bufferIndex] = KEYBOARD_BUFFER[bufferIndex];
   } 
   buffer[bufferIndex+1] = 0;
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

   //Write the key to the screen and place it in
   //the buffer
   if( kb_status & 0x01 ){
      key = portb_read( 0x60 );
      if( key < 0 )
	      return;
      k_putchar( keyboard_map[(int)key] );
      add_keyboard_buffer( keyboard_map[(int)key]);
   }

}
