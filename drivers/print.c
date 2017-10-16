
#include "vgaentry.h"
#include "portio.h"

#define NEWLINE 10
#define ROWS 25
#define COLS 80

#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

char *VIDEO_MEMORY = (char*)0xb8000;
int CUR_SCREEN_OFFSET = 0;

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

void k_printfull(char *msg, enum vga_color color){

   int i;
   for(i = 0; msg[i] != 0; i++ ){
      VIDEO_MEMORY[CUR_SCREEN_OFFSET] = msg[i];
      VIDEO_MEMORY[CUR_SCREEN_OFFSET + 1] = color;
      CUR_SCREEN_OFFSET += 2;  //Skip two since each space on the
                               //screen has two bytes.
   }
}

/*void k_printfull(char *msg, enum vga_color color, int offset){

   unsigned int current_line = (int)( CUR_SCREEN_OFFSET / 160 );
   unsigned int startPrintingAt = current_line * 160 + offset * 2;

   int i;
   for(i = 0; msg[i] != 0; i++ ){
      VIDEO_MEMORY[startPrintingAt + i * 2] = msg[i];
      VIDEO_MEMORY[startPrintingAt + i * 2 + 1] = color;
   }
}*/



//k_printfull with defaults
void k_print(char *msg){
   k_printfull(msg, make_color(COLOR_WHITE, COLOR_BLACK));
}

