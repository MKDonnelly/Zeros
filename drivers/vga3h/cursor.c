#include "vga3h.h" 

int vga3h_get_cursor(){

   //Get the position of the cursor

   //reg 14 is high byte of cursor offset
   //reg 15 is low byte of cursor offset
   portb_write(SCREEN_CTRL_PORT, 14);
   int offset = portb_read(SCREEN_DATA_PORT) << 8;
   portb_write(SCREEN_CTRL_PORT, 15);
   offset += portb_read(SCREEN_DATA_PORT);

   //Number of chars is returned, so multiply by 2
   return offset * 2;
}


//Given an x and y value, move the
//cursor to that location
void vga3h_move_cursor(int x, int y){
   vga3h_move_cursorl( k_xy_to_linear( x, y ) );
}

//Move cursor using a linear offset
void vga3h_move_cursorl(int position){
   //Send a command to the vga controller
   //with registers 14 and 15
   //with the high and low bytes to set.
   portb_write(SCREEN_CTRL_PORT, 14);
   portb_write(SCREEN_DATA_PORT, position >> 8); //send upper bits
   portb_write(SCREEN_CTRL_PORT, 15);
   portb_write(SCREEN_DATA_PORT, position);      //send lower bits
}

//Convert a x,y pair of coordinates on the screen
//to a linear address in video memory
int k_xy_to_linear(int x, int y){
   return y * 80 + x;
}

