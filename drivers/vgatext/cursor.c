#define ROWS 25
#define COLS 80

#include "../../drivers/portio.h"

#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

int get_cursor(){

   //Get the position of the cursor

   //reg 14 is high byte of cursor offset
   //reg 15 is low byte of cursor offset
   portb_write(REG_SCREEN_CTRL, 14);
   int offset = portb_read(REG_SCREEN_DATA) << 8;
   portb_write(REG_SCREEN_CTRL, 15);
   offset += portb_read(REG_SCREEN_DATA);

   //Number of chars is returned, so multiply by 2
   return offset * 2;
}


//Given an x and y value, move the
//cursor to that location
void move_cursor(int x, int y){

   unsigned tmp = y * 80 + x;

   //Send a command to the vga controller
   //with registers 14 and 15
   //with the high and low bytes to set.
   portb_write(REG_SCREEN_CTRL, 14);
   portb_write(REG_SCREEN_DATA, tmp >> 8); //send upper bits
   portb_write(REG_SCREEN_CTRL, 15);
   portb_write(REG_SCREEN_DATA, tmp);      //send lower bits

}

void move_cursorl(int position){

   //Send a command to the vga controller
   //with registers 14 and 15
   //with the high and low bytes to set.
   portb_write(REG_SCREEN_CTRL, 14);
   portb_write(REG_SCREEN_DATA, position >> 8); //send upper bits
   portb_write(REG_SCREEN_CTRL, 15);
   portb_write(REG_SCREEN_DATA, position);      //send lower bits

}

//Convert a x,y pair of coordinates on the screen
//to a linear address in video memory
int k_xy_to_linear(int x, int y){
   return y * 80 + x;
}

