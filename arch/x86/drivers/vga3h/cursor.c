#include <arch/x86/drivers/vga3h/vga3h.h>

int vga3h_get_cursor(){

   //Get the position of the cursor

   //reg 14 is high byte of cursor offset
   //reg 15 is low byte of cursor offset
   portb_write(SCREEN_CTRL_P, VGA3H_CURSOR_HIGH_P);
   int offset = portb_read(SCREEN_DATA_P) << 8;
   portb_write(SCREEN_CTRL_P, VGA3H_CURSOR_LOW_P);
   offset += portb_read(SCREEN_DATA_P);

   //Number of chars is returned, so multiply by 2
   return offset * 2;
}


//Given an x and y value, move the
//cursor to that location
void vga3h_move_cursor(int x, int y){
   vga3h_move_cursorl( vga3h_xy_to_linear( x, y ) );
}

//Move cursor using a linear offset
void vga3h_move_cursorl(int position){
   //Send a command to the vga controller
   //with registers 14 and 15
   //with the high and low bytes to set.
   portb_write(SCREEN_CTRL_P, VGA3H_CURSOR_HIGH_P);
   portb_write(SCREEN_DATA_P, position >> 8); //send upper bits
   portb_write(SCREEN_CTRL_P, VGA3H_CURSOR_LOW_P);
   portb_write(SCREEN_DATA_P, position);      //send lower bits
}

//Convert a x,y pair of coordinates on the screen
//to a linear address in video memory
int vga3h_xy_to_linear(int x, int y){
   return y * VGA3H_COLS + x;
}

