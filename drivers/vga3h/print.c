#include "vga3h.h"

char *VGA3H_VIDEO_MEMORY = (char*)0xb8000;
//Current screen offset as linear offset
int VGA3H_CUR_CURSOR_OFFSET = 0;
int VGA3H_CUR_SCREEN_OFFSET = 0;

//Place a character on the screen at the
//current screen offset. All other print
//functions should call this to actually
//place the character on the screen.
void vga3h_putchar( char input ){
   if( input == '\n' || input == '\r' ){
      vga3h_newline();
   }else if( input == '\b' ){
      //Backspace handling
      VGA3H_CUR_SCREEN_OFFSET -= 2;
      VGA3H_CUR_CURSOR_OFFSET--;
      vga3h_move_cursorl( VGA3H_CUR_CURSOR_OFFSET );
      VGA3H_VIDEO_MEMORY[VGA3H_CUR_SCREEN_OFFSET] = ' ';
   }else{
      VGA3H_VIDEO_MEMORY[VGA3H_CUR_SCREEN_OFFSET] = input;
      VGA3H_CUR_SCREEN_OFFSET += 2; //Skip 2 bytes since 
                           //the second byte is for color
      VGA3H_CUR_CURSOR_OFFSET++;    //Update the cursor position
      vga3h_move_cursorl( VGA3H_CUR_CURSOR_OFFSET ); 
   }
}

void vga3h_putchar_at( char input, int x, int y ){

   //We need to multiply both parts by 2 since
   //every position has both a character and a color
   //attribute.
   int startIndex = y * VGA3H_COLS * 2 + x * 2;   
   //We don't need to handle \n or \r characters here
   //since we only keep the current cursor position with
   //vga3h_putchar.
   VGA3H_VIDEO_MEMORY[startIndex] = input;
}

//This functions alters SCREEN_LOCATION to
//move it to the next line. There is a new
//line every 160 positions.
void vga3h_newline(){
  int current_line = (int)( VGA3H_CUR_SCREEN_OFFSET / (VGA3H_COLS*2) );
  VGA3H_CUR_SCREEN_OFFSET = ( (current_line + 1) * (VGA3H_COLS*2)); 

  //Also modify the current cursor location
  //Add 1 to the y value since we want to go
  //to the next line.
  vga3h_move_cursor( 0, current_line + 1);
  VGA3H_CUR_CURSOR_OFFSET = vga3h_xy_to_linear( 0, current_line + 1 );
}


//Clear the screen and update the position
void vga3h_clear_screen( ){
   VGA3H_CUR_SCREEN_OFFSET = 0;
   for( int i = 0; i < VGA3H_ROWS * VGA3H_COLS; i++){
      vga3h_putchar(' ');
   }
   VGA3H_CUR_SCREEN_OFFSET = 0;
}
