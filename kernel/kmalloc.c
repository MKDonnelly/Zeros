#include "kmalloc.h"

//Heap will start at 1MB mark...
udword start_free_mem = 0x200000;
//...and end at 2MB mark
udword end_free_mem = 0x300000;

void *kmalloc(int size, int align){
   if( start_free_mem + size < end_free_mem ){
      if( align ){
        start_free_mem &= 0xFFFFF000;  //Align the allocated memory
	start_free_mem += 0x1000;      //Add 4k to prevent overlap
      }
      udword allocated = start_free_mem;
      start_free_mem += size;
      return (void*)allocated;
   }
   //Cannot allocate any more memory.
   return 0;
}
