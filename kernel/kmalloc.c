#include "kmalloc.h"

//Heap will start at 4MB mark...
unsigned int kernel_start_heap = 0x400000;
//...and end at 6MB mark
unsigned int kernel_end_heap = 0x600000;

void *kmalloc(int size, int align, unsigned int *phys){
   if( kernel_start_heap + size < kernel_end_heap ){
      if( align ){
        kernel_start_heap &= 0xFFFFF000;  //Align the allocated memory
	kernel_start_heap += 0x1000;      //Add 4k to prevent overlap
      }
      if( phys ){
         *phys = kernel_start_heap;
      }
      udword allocated = kernel_start_heap;
      kernel_start_heap += size;
      return (void*)allocated;
   }
   return (void*)0; //Cannot allocate any more memory
}
