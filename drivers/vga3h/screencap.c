#include "vga3h.h"

//Array is assumed to be 24x160
void vga3h_screen_cap(int captureArray[][160]){

   for(int row = 0; row < VGA3H_ROWS; row++){
      for(int col = 0; col < VGA3H_COLS; col++){
         captureArray[row][col] = VGA3H_VIDEO_MEMORY[160 * row + col];
      }
   }
}


void vga3h_screen_res(int restoreArray[][160]){

   for(int row = 0; row < VGA3H_ROWS; row++){
      for(int col = 0; col < VGA3H_COLS; col++){
         VGA3H_VIDEO_MEMORY[160 * row + col] = restoreArray[row][col];
      }
   }
}

