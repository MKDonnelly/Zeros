#include "kmalloc.h"

//Heap will start at 3MB mark...
unsigned int kernel_start_heap = 0x300000;
//...and end at 5MB mark
unsigned int kernel_end_heap = 0x500000;

//Initilize the first heapNode in the heap.
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
//NOTE: To make it easier on ourselves when aligning 
//      on 4K boundaries, this function will often 
//      just add 4K as an overestimate instead of going through
//      the calculation to get it perfect.
void *kmalloc(int size, int align, unsigned int *phys){

   struct heapNode* head = (struct heapNode*)kernel_start_heap;
   int retAddr = 0; //The address of free memory to allocate.
                    //This will not be changed if free memory
                    //could not be found

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

      //Make sure we are dealing with enough memory in this chunk
      //i.e. The while loop above did not break out because we hit
      //the end and head->size is not big enough.
      if( ! head->nextChunk || (head->size >= (size + 0x1000) )){

         //Information about where the free memory in the block will
         //start, where the 4K ALIGNED memory will start within this
         //block, and the total size needed after aligning to 4K.
         int memBlockStart = (int)head + sizeof(struct heapNode);
         int alignedStart = ((int)head + sizeof(struct heapNode) + 0x1000)
                             & 0xFFFFF000;
         int totalSize = size + (alignedStart - memBlockStart);
         ///

         struct heapNode *curItem = head;
         struct heapNode *nextItem = (struct heapNode*)memBlockStart;      

         nextItem->nextChunk = curItem->nextChunk;
         nextItem->size = curItem->size - sizeof(struct heapNode) - totalSize;
         nextItem->isAllocated = 0;
   
         curItem->nextChunk = nextItem;
         curItem->size = totalSize;
         curItem->isAllocated = 1;

         //This will find the first byte of usable memory in the chunk
         //and align the free space to 4K boundaries.
         retAddr = (memBlockStart + 0x1000) & 0xFFFFF000;
  
         if( phys )
            *phys = retAddr;
      }
   //We do not need to align on 4K
   }else{
      while( head->size < size && head->nextChunk ){
           head = head->nextChunk;
      }

      //Make sure we are dealing with a valid chunk of free memory
      if( head->size >= size ){
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
   }
   
   return (void*)retAddr;
}


//Free an allocated chunk of memory
void kfree(void *memChunk){

   struct heapNode *head = (struct heapNode*)kernel_start_heap;
   char foundMem = 0;

   while( head->nextChunk && ! foundMem){

      //See if the memChunk address falls in the free 
      //space of the current chunk. If it does, we allocate
      //it and break out of the loop by setting foundMem.
      int chunkStart = ( (int)head + sizeof( struct heapNode ) );
      int chunkEnd = ( (int)head + sizeof(struct heapNode) + head->size );

      if( (int)memChunk <= chunkEnd && (int)memChunk >= chunkStart){
         head->isAllocated = 0;
         foundMem = 1;
      }else{
         head = head->nextChunk;
      }
   }

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

/*
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
}*/

