#include "keyboard.h"

char keyboard_buffer[KEYBOARD_BUFFER_MAX];
int keyboard_number_chars = 0;

char callback_key;
void (*char_callback)(char);

void keyboard_main_handler(char key_entered){

   if( key_entered == callback_key ){
      char_callback(key_entered);
   }

   //We have space in the buffer.
   if( keyboard_number_chars < KEYBOARD_BUFFER_MAX ){
      keyboard_buffer[keyboard_number_chars++] = key_entered;
   }else{
   //We do not have space in the buffer. Move everything down
   //by one space and get rid of the oldest key.
      for(int i = 0; i < (KEYBOARD_BUFFER_MAX-1); i++){
         keyboard_buffer[i] = keyboard_buffer[i+1];
      }
      keyboard_buffer[keyboard_number_chars] = key_entered;
   }
}

void keyboard_register_key_callback( void (*key_callback)(char), char key){
   callback_key = key;
   char_callback = key_callback;
}
