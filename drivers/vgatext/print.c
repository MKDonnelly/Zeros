#include "vgatext.h"

char *VIDEO_MEMORY = (char*)0xb8000;
//Current screen offset as linear offset
int CUR_SCREEN_OFFSET = 0;  
int CUR_CURSOR_OFFSET = 0;

//Place a character on the screen at the
//current screen offset. All other print
//functions should call this to actually
//place the character on the screen.
void k_putchar( char input ){
   VIDEO_MEMORY[CUR_SCREEN_OFFSET] = input;
   CUR_SCREEN_OFFSET += 2; //Skip 2 bytes since 
                           //the second byte is for color
   CUR_CURSOR_OFFSET++;    //Update the cursor position
   move_cursorl( CUR_CURSOR_OFFSET ); 
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
   int i = 0;
   while( s[i] != 0 ){
      k_putchar( s[i] );
      i++;
   }
}
