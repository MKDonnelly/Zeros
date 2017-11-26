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
}

//Walk the heap until enough memory is found
//Align aligns the page on 4K boundaries. 
void *exp_kmalloc(int size, int align, unsigned int *phys){

   struct heapNode* head = (struct heapNode*)kernel_start_heap;
   int retAddr = 0;

   if( align ){
      //NOTE: We will simply overestimate the space
      //needed by adding 4K (0x1000). The most space
      //that could be wasted after aligning the memory
      //to 4K boundaries is 0x1000 - 1 bytes. 
      //Loop while the current chunk is not large enough and
      //we have not hit the end.
      while( head->size < ( size + 0x1000 ) && head->nextChunk ){
           head = head->nextChunk;
      }

      //For the time being, we will assume that we are at a valid chunk.
      struct heapNode *curItem = head;
      struct heapNode *nextItem = (struct heapNode*)( (int)head + sizeof( struct heapNode ) );
      
      nextItem->nextChunk = curItem->nextChunk;
      nextItem->size = curItem->size - sizeof( struct heapNode ) - size;
      nextItem->isAllocated = 0;

      curItem->nextChunk = nextItem;
      curItem->size = size;
      curItem->isAllocated = 1;

      //This will find the first byte of usable memory in the chunk
      //and align the free space to 4K boundaries.
      retAddr = ((int)head + sizeof( struct heapNode ) + 0x1000) & 0xFFFFF000;

      if( phys )
         *phys = retAddr;

   }else{
      while( head->size < size && head->nextChunk ){
           head = head->nextChunk;
      }

      //For the time being, we will assume that we are at a valid chunk.
      struct heapNode *curItem = head;
      struct heapNode *nextItem = (struct heapNode*)( (int)head + sizeof( struct heapNode ) );
      
      nextItem->nextChunk = curItem->nextChunk;
      nextItem->size = curItem->size - sizeof( struct heapNode ) - size;
      nextItem->isAllocated = 0;

      curItem->nextChunk = nextItem;
      curItem->size = size;
      curItem->isAllocated = 1;

      if( phys )
         *phys = retAddr;

      retAddr = ((int)head + sizeof( struct heapNode ) );
   }
   
   return (void*)retAddr;
}


//Free an allocated chunk of memory
void exp_kfree(void *memChunk){
   struct heapNode *head = (struct heapNode*)kernel_start_heap;
   char foundMem = 0;
   while( head->nextChunk && ! foundMem){
      //We have found the memory chunk being used
      if( (int)( (int)head + sizeof(struct heapNode)) == (int)memChunk){
         foundMem = 1;
      }else{
         head = head->nextChunk;
      }
   }
   //Deallocate the block
   head->isAllocated = 0;   

   //Yes, this is very inefficient, 
   //but it is simple.
   unify_heap();
}

//Run through the heap and merge as much
//free memory as possible
void unify_heap(){
   struct heapNode* head = (struct heapNode*)kernel_start_heap;
   while( head->nextChunk ){
      //If we come across two ajacent free memory segments, unify them.
      if( head->isAllocated == 0 && head->nextChunk->isAllocated == 0 ) {
         head->size += head->nextChunk->size + sizeof( struct heapNode );
         head->nextChunk = head->nextChunk->nextChunk;
      }
   }
}

