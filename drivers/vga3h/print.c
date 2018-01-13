#include "vga3h.h"

char *VGA3H_VIDEO_MEMORY = (char*)0xb8000;
//Current screen offset as linear offset
int VGA3H_CUR_CURSOR_OFFSET = 0;
int VGA3H_CUR_SCREEN_OFFSET = 0;

//The default foreground and background 
//colors.
int VGA3H_CUR_FG_COLOR = VGA3H_WHITE;
int VGA3H_CUR_BG_COLOR = VGA3H_BLACK;


char vga3h_color_array[] = {
    VGA3H_BLACK,
    VGA3H_BLUE,
    VGA3H_GREEN,
    VGA3H_CYAN,
    VGA3H_RED,
    VGA3H_MAGENTA,
    VGA3H_BROWN,
    VGA3H_LIGHT_GREY,
    VGA3H_DARK_GREY,
    VGA3H_LIGHT_BLUE,
    VGA3H_LIGHT_GREEN,
    VGA3H_LIGHT_CYAN,
    VGA3H_LIGHT_RED,
    VGA3H_LIGHT_MAGENTA,
    VGA3H_LIGHT_BROWN,
    VGA3H_WHITE
};


//Print a character to the screen. Automatically
//keep track of which spaces have been used and move
//the cursor forward
void vga3h_putchar(char character){

   int colorToUse = vga3h_make_color( VGA3H_CUR_FG_COLOR,
                                      VGA3H_CUR_BG_COLOR );

   if( character == '\n' || character == '\r' ){
      vga3h_newline();
   }else if( character == '\b' ){
      //Backspace handling
      
      //Move back one cell and reset
      //the color byte
      VGA3H_CUR_SCREEN_OFFSET--;
      VGA3H_VIDEO_MEMORY[VGA3H_CUR_SCREEN_OFFSET] = colorToUse;

      //Move back one more cell and erase the
      //character byte
      VGA3H_CUR_SCREEN_OFFSET--;
      VGA3H_VIDEO_MEMORY[VGA3H_CUR_SCREEN_OFFSET] = ' ';

      //Finally, update the cursor offset and position
      //Note that the cursor offset tracks the number of
      //visible spaces on the screen, not the number of cells
      VGA3H_CUR_CURSOR_OFFSET--;
      vga3h_move_cursorl( VGA3H_CUR_CURSOR_OFFSET );
   }else{

      //Set the character byte and move one ahead
      VGA3H_VIDEO_MEMORY[VGA3H_CUR_SCREEN_OFFSET] = character;
      VGA3H_CUR_SCREEN_OFFSET++;

      //Set the color byte and move one ahead
      VGA3H_VIDEO_MEMORY[VGA3H_CUR_SCREEN_OFFSET] = colorToUse;
      VGA3H_CUR_SCREEN_OFFSET++; 

      //Advance the cursor one space 
      VGA3H_CUR_CURSOR_OFFSET++;    
      vga3h_move_cursorl( VGA3H_CUR_CURSOR_OFFSET ); 
   }
}


//Put a character at the given location
void vga3h_putchar_at( char character, int x, int y ){

    int colorToUse = vga3h_make_color( VGA3H_CUR_FG_COLOR, VGA3H_CUR_BG_COLOR );

   //We need to multiply both parts by 2 since
   //every position has both a character and a color
   //attribute.
   int startIndex = y * VGA3H_COLS * 2 + x * 2;   

   //We don't need to handle \n or \r characters here
   //since we only keep the current cursor position with
   //vga3h_putchar.
   VGA3H_VIDEO_MEMORY[startIndex] = character;
   VGA3H_VIDEO_MEMORY[startIndex+1] = colorToUse;
}

//This functions alters VGA3H_CUR_SCREEN_OFFSET to
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
//Just doing a memset() would have been nice,
//but unfortunatelly, all of the character spaces
//would have been set to display \0, and the BG
//and FG colors would have been black and black!
void vga3h_clear_screen( ){
   VGA3H_CUR_SCREEN_OFFSET = 0;
   for( int i = 0; i < VGA3H_ROWS * VGA3H_COLS; i++){
      vga3h_putchar(' ');
   }
   VGA3H_CUR_SCREEN_OFFSET = 0;
}

//Skip the many function calls to putchar
void vga3h_fast_cls(){
   int colorToUse = vga3h_make_color( VGA3H_CUR_FG_COLOR,
                                      VGA3H_CUR_BG_COLOR );
   for(int start = 0, end = VGA3H_ROWS * VGA3H_COLS * 2;
                             start < end ; start+=2){
      VGA3H_VIDEO_MEMORY[start] = ' ';
      VGA3H_VIDEO_MEMORY[start+1] = colorToUse;
   } 
}


void vga3h_set_bg_color(int bg){
   VGA3H_CUR_BG_COLOR = bg;
}

void vga3h_set_fg_color(int fg){
   VGA3H_CUR_FG_COLOR = fg;
}

int vga3h_get_bg_color(){
   return VGA3H_CUR_BG_COLOR;
}

int vga3h_get_fg_color(){
   return VGA3H_CUR_FG_COLOR;
}
