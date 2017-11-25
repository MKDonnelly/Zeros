#include "kmalloc.h"

//Heap will start at 3MB mark...
unsigned int kernel_start_heap = 0x300000;
//...and end at 5MB mark
unsigned int kernel_end_heap = 0x500000;

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

//                Experimental heap allocator
/////////////////////////////////////////////

void heap_init(){
    //Initilize the heap by creating the first
    //linked list node at the start of the heap
    ((struct heapNode*)kernel_start_heap)->nextChunk = 0; //Null it out
    ((struct heapNode*)kernel_start_heap)->size = kernel_end_heap - 
                                                kernel_start_heap - 
                                                sizeof( struct heapNode );
    ((struct heapNode*)kernel_start_heap)->isAllocated = 0; //Not allocated
    
    //
    ((struct heapNode*)kernel_start_heap)->not_used = 0xF;
}

//Walk the heap until enough memory is found
void *exp_kmalloc(int size){


   struct heapNode* head = (struct heapNode*)kernel_start_heap;
   //Keep looping until we have found a large enough chunk
   //or run into the end of the heap.
   while( head->size < size && ! head->nextChunk ){
      head = (struct heapNode*)head->nextChunk;
   }

   //Find where the next node will be placed
   //and initilize it
   struct heapNode *next = (struct heapNode*)((int)head + size + sizeof( struct heapNode ));
   next->nextChunk = head->nextChunk;
   next->size = head->size - sizeof( struct heapNode ) - size;
   next->isAllocated = 0;
   next->not_used = head->not_used; 
   

   //Now we are free to 
   head->nextChunk = next;
   head->size = size;
   head->isAllocated = 1;
   head->not_used = 0xA;

   //Return the pointer to the memory to use 
   return (void*)((int)head + sizeof(struct heapNode));
}



