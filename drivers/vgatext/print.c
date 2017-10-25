#include "print.h"

//Place a character on the screen at the
//current screen offset.
void k_putchar( char input ){
   VIDEO_MEMORY[CUR_SCREEN_OFFSET] = input;
   CUR_SCREEN_OFFSET += 2; //Skip 2 bytes since 
                           //the second byte is for color
}

//Clear the screen and update the position
void k_clear_screen( ){
   CUR_SCREEN_OFFSET = 0;
   for( int i = 0; i < ROWS * COLS; i++){
      k_putchar(' ');
   }
   CUR_SCREEN_OFFSET = 0;
}

//This functions alters SCREEN_LOCATION to
//move it to the next line. There is a new
//line every 160 positions.
void k_newline(){
  int current_line = (int)( CUR_SCREEN_OFFSET / 160 );
  CUR_SCREEN_OFFSET = ( (current_line + 1) * 160); 
}

void k_print( char *s ){
   int i = 0, j = 0;
   char *vm = (char*)0xb8000;
   while( s[j] != 0 ){
      vm[i] = s[j];
      CUR_SCREEN_OFFSET += 2;
      j++;
      i += 2;
   }
}
