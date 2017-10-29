#include "kmalloc.h"

udword start_free_mem = 0x1000;

void *kmalloc(int size){
   int retVal = start_free_mem;
   start_free_mem += size;
   return (void*)retVal;
}
