#include <arch/x86/portio.h>

//TODO convert to pure assembly

int8_t portb_read(uint16_t port) {

   //"=a" (result) means put AL register in variable result when done
   // "d" (port) means load edx with port
   unsigned char result;
   asm("in %%dx, %%al" : "=a" (result) : "d" (port) );
   return result;
}

void portb_write(uint16_t port, uint16_t data){

   // "a" (data) means load eax with data
   // "d" (port) means load edx with port
   asm( "out %%al, %%dx" : : "a" (data), "d" (port));
}

uint16_t portw_read(uint16_t port){

   unsigned short result;
   asm("in %%dx, %%ax" : "=a" (result) : "d" (port));
   return result;
}

void portw_write(uint16_t port, uint16_t data){
   asm( "out %%ax, %%dx" : : "a" (data), "d" (port));
}
