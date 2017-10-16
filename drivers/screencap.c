#include "print.h"
extern char* VIDEO_MEMORY;


//Array is assumed to be 24x160
void k_screen_cap(int captureArray[][160]){

   int rows = 24, cols = 80;

   for(int row = 0; row < rows; row++){

      for(int col = 0; col < cols; col++){
         captureArray[row][col] = VIDEO_MEMORY[160 * row + col];
      }
   }
}


void k_screen_res(int restoreArray[][160]){

   int rows = 24, cols = 80;

   for(int row = 0; row < rows; row++){

      for(int col = 0; col < cols; col++){
         VIDEO_MEMORY[160 * row + col] = restoreArray[row][col];
      }
   }
}

