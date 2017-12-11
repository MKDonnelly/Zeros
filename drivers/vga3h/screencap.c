#include "vga3h.h"

//Array is assumed to be at least VGA3H_ROWS * VGA3H_COLS
void vga3h_screen_cap(char *captureArray){
   for(int i = 0; i < VGA3H_ROWS * VGA3H_COLS; i++){
      captureArray[i] = VGA3H_VIDEO_MEMORY[i];
   }
}


void vga3h_screen_res(char *restoreArray){
   for(int i = 0; i < VGA3H_ROWS * VGA3H_COLS; i++){
      VGA3H_VIDEO_MEMORY[i] = restoreArray[i];
   }
}

