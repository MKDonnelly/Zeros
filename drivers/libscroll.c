
#include "libprint.h"
extern char* VIDEO_MEMORY;


void scroll(){
   unsigned int max = 80 * 24 * 2;

   //Since each character is 80 spaces (160 slots)
   //away from the space directly above, the screen
   //can be run through linearly
   int to, from;
   for(to = 0, from = 160; to <= max; to++, from++){
      VIDEO_MEMORY[to] = VIDEO_MEMORY[from];
   }
}

void scroll_by(int n){
   for(int i = 0; i < n; i++)
      scroll();
}

