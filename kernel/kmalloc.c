#include "kmalloc.h"

//Heap will start at 3MB mark...
unsigned int kernel_start_heap = 0x300000;
//...and end at 5MB mark
unsigned int kernel_end_heap = 0x500000;

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
void *kmalloc(int size, int align, unsigned int *phys){

   struct heapNode* head = (struct heapNode*)kernel_start_heap;
   //Keep looping until we have found a large enough chunk
   //or run into the end of the heap.

   char foundBlock = 0;
   int requiredSize = size;

   do{
      //If we need alignment, check to make sure
      //we have enough space.
      if( align ){
         int chunkAddr = (int)((int)head + sizeof( struct heapNode ) );
         //Align to 4K, add 4K to be safe, add the size, and 
         requiredSize = ((chunkAddr & 0xFFFFF000) + 0x1000 + size) - chunkAddr;
         if( head->size >= requiredSize ){
             foundBlock = 1;
         }
      }else{
         //No alignment, this is really easy.
         if( head->size >= size ){
            foundBlock = 1;
         }
      }
      //Only skip to the next block if the
      //current one will not work.
      if( ! foundBlock )
         head = head->nextChunk;
   }while( head->nextChunk && ! foundBlock);


   //At this point, head points to a suitable chunk of memory in
   //the heap. Split the current block (if needed).
   
   //Determine if we need another block
   if( (requiredSize + sizeof( struct heapNode )) > head->size){
      //We do not have enough space to allocate another block.
      head->isAllocated = 1;  
   }else{
      //We can split this block up
      //Find where the next node will be placed
      //and initilize it
      struct heapNode *next = (struct heapNode*)((int)head + requiredSize + sizeof( struct heapNode ));
     
      next->nextChunk = head->nextChunk;
      next->size = head->size - sizeof( struct heapNode ) - requiredSize;
      next->isAllocated = 0;
   
      //Now we are free to modify the head block
      head->nextChunk = next;
      head->size = requiredSize;
      head->isAllocated = 1;
   }

   if( phys )
      *phys = (unsigned int)( (int)head + sizeof(struct heapNode) );


   //Return the pointer to the memory to use 
   if( align ){
      return (void*)( ((int)head + sizeof(struct heapNode)) & 0xFFFFF000 + 0x1000);
   }else{
      return (void*)((int)head + sizeof(struct heapNode));
   }
}

/*
//Walk the heap until enough memory is found
void *exp_kmalloc(int size, int align){

   struct heapNode* head = (struct heapNode*)kernel_start_heap;
   //Keep looping until we have found a large enough chunk
   //or run into the end of the heap.

   while( head->size < size && ! head->nextChunk ){
      head = head->nextChunk;
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
*/

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

//Fun through the heap and merge as much
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

