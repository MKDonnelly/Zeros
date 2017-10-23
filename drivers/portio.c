
#include "portio.h"

sbyte portb_read(uword port) {

   //"=a" (result) means put AL register in variable result when done
   // "d" (port) means load edx with port
   unsigned char result;
   __asm__("in %%dx, %%al" : "=a" (result) : "d" (port) );
   return result;
}

void portb_write(uword port, uword data){

   // "a" (data) means load eax with data
   // "d" (port) means load edx with port
   __asm__( "out %%al, %%dx" : : "a" (data), "d" (port));
}

uword portw_read(uword port){

   unsigned short result;
   __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
   return result;
}

void portw_write(uword port, uword data){
   __asm__( "out %%ax, %%dx" : : "a" (data), "d" (port));
}
