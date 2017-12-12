#include "kmalloc.h"

//Heap will start at 3MB mark...
uint32_t kernel_start_heap = 0x300000;
//...and end at 5MB mark
uint32_t kernel_end_heap = 0x500000;

//Initilize the first heapNode in the heap.
void init_heap(){
    struct heapNode* head = (struct heapNode*)kernel_start_heap;

    //Initilize the first element by setting its size to
    //the amount of memory, the next node to 0, and 
    //is allocated to 0.
    head->nextNode = 0; //No next node, this is the first 
    head->size = kernel_end_heap - kernel_start_heap - HEAPNODE_SIZE;
    head->freeMem = ( (uint8_t*)head + HEAPNODE_SIZE);
    head->isAllocated = 0;
}


//Walk the heap until enough memory is found
//If "align" is 0, do not align to 4k, else align. 
//NOTE: To make it easier on ourselves when aligning 
//      on 4K boundaries, this function will just  
//      add 4K as an overestimate instead of going through
//      the calculation to get a perfect fit.
void *kmalloc(uint32_t size, uint8_t align, uint32_t *phys){

   struct heapNode* head = (struct heapNode*)kernel_start_heap;

   //Align on 4K boundary
   if( align ){
      //NOTE: We will simply overestimate the space
      //needed by adding 4K (0x1000). The most space
      //that could be wasted after aligning the memory
      //to 4K boundaries is 4k - 1 bytes. Loop while
      //the current node is allocated or the current node
      //is not big enough
      while( head->isAllocated || ( (head->size < ( size + 0x1000 ) && head->nextNode))){
           head = head->nextNode;
      }

      //Check to make sure the node we landed on is valid.
      //If it is not, we probably ran out of memory
      if( ! (head->isAllocated || ((head->size < (size+0x1000)) && (head->nextNode)))){

         struct heapNode *curNode = head;
         struct heapNode *nextNode = (struct heapNode*)( (uint8_t*)head + size + 0x1000 );

         //Imagine we have a block of 1000 free byte we are using.
         //Now support we want 990 of those bytes. It does not make
         //sense to take the last 10 bytes, create a heapnode with it,
         //and still have free space left over. In the case where we
         //have less than 20 bytes to work with (excluding the heapNode
         //header), it is easier to just merge this will the current node
         if( curNode->size - size - HEAPNODE_SIZE < MIN_SPLIT ){
            //No extra heapnode needed
            curNode->isAllocated = 1;
            curNode->freeMem = (void*)((uint8_t*)curNode + HEAPNODE_SIZE);
            return curNode->freeMem;

         }else{
            //Create a heapnode

            //Initilize the nextNode
            nextNode->nextNode = curNode->nextNode;
            nextNode->freeMem = ( (uint8_t*)nextNode + HEAPNODE_SIZE );
            nextNode->size = curNode->size - HEAPNODE_SIZE - size - 0x1000;
            nextNode->isAllocated = 0; //Again, assume that the block we are
                                       //sitting on is unallocated

            //Initilize the current node
            curNode->nextNode = nextNode;
            //Get the head of the current node, add HEAPNODE_SIZE to get
            //to the first byte of free memory, add 4K since we
            //just overestimate, and align it all to 4K.
            curNode->freeMem = (void*)(((uint32_t)((uint8_t*)curNode + HEAPNODE_SIZE + 0x1000)) & 0xFFFFF000);
            curNode->size = size + 0x1000;
            curNode->isAllocated = 1;
      
            //If specified, return the address
            if( phys )
               *phys = (int)curNode->freeMem;

            return curNode->freeMem;
         }
      }

   //We do not need to align on 4K
   }else{

      //Go through each block while the current block is allocated or
      //is not large enough
      while( head->isAllocated || (head->size < size && head->nextNode)){
           head = head->nextNode;
      }

      //Make sure we are dealing with a valid chunk of free memory
      if( ! ( head->isAllocated || (head->size < size && head->nextNode) )){

         //It only makes sense to split the free space into two 
         //heapnodes if we have enough space. See above
         if( head->size - size - HEAPNODE_SIZE < MIN_SPLIT ){
            //No heapnode required
            head->freeMem = (void*)( (uint8_t*)head + HEAPNODE_SIZE );
            head->isAllocated = 1;
            return head->freeMem;
         }else{
            //Split into two sections

            struct heapNode *curItem = head;
            struct heapNode *nextItem = (struct heapNode*)( (uint8_t*)head + HEAPNODE_SIZE + size);
      
            nextItem->nextNode = curItem->nextNode;
            nextItem->freeMem = ( (uint8_t*)nextItem + HEAPNODE_SIZE );
            nextItem->size = curItem->size - sizeof( struct heapNode ) - size;
            nextItem->isAllocated = 0;
 
            curItem->nextNode = nextItem;
            curItem->size = size;
            curItem->freeMem = ( (uint8_t*)curItem + HEAPNODE_SIZE );
            curItem->isAllocated = 1;

            if( phys )
               *phys = (int)curItem->freeMem;

            return curItem->freeMem;
         }
      }
   }
   
   //If we get here, we must have run out of memory.
   return NULL;
}

//Run through the heap and merge as much
//free memory as possible. 
//TODO Eliminate this and have a kfree look
//at the node before and after it.
static void unify_heap(){
   struct heapNode* head = (struct heapNode*)kernel_start_heap;
   while( head->nextNode ){
      //If we come across two ajacent free memory segments, unify them.
      if( head->isAllocated == 0 && head->nextNode->isAllocated == 0 ) {
         head->size += head->nextNode->size + HEAPNODE_SIZE;
         head->nextNode = head->nextNode->nextNode;
      }
   }
}

//Free an allocated chunk of memory
void kfree(void *memChunk){

   struct heapNode *head = (struct heapNode*)kernel_start_heap;
   char foundMem = 0;

   while( head->nextNode && ! foundMem){

      //See if the memChunk address falls in the free 
      //space of the current chunk. If it does, we allocate
      //it and break out of the loop by setting foundMem.
      uint32_t chunkStart = (uint32_t)((uint8_t*)head + HEAPNODE_SIZE);
      uint32_t chunkEnd = (uint32_t)((uint8_t*)head + HEAPNODE_SIZE + head->size);

      if( (int)memChunk <= chunkEnd && (int)memChunk >= chunkStart){
         //If we found the chunk, all we need to do it
         //set the isAllocated flag to 0.
         head->isAllocated = 0;
         foundMem = 1;
      }else{
         head = head->nextNode;
      }
   }

   //Yes, this is very inefficient, 
   //but it is simple.
   unify_heap();
}
