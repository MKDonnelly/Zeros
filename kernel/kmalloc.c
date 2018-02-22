#include <kernel/kmalloc.h>

//Heap will start at 3MB mark...
uint32_t kernel_start_heap = 0x300000;
//...and end at 5MB mark
uint32_t kernel_end_heap = 0x500000;

//Initilize the first heapNode in the heap.
void init_heap(){
    heapnode_t* head = (heapnode_t*)kernel_start_heap;

    //Initilize the first element by setting its size to
    //the amount of memory, the next node to 0, and 
    //is allocated to 0.
    head->next_node = NULL; //No next node, this is the first 
    head->size = kernel_end_heap - kernel_start_heap - sizeof(heapnode_t);
    head->free_mem = ( (uint8_t*)head + sizeof(heapnode_t) );
    head->allocated = 0;
}


//Allocate "size" piece of memory on the heap. If align is non-zero
//align the memory to it's size. If phys is non-null, return the physical
//address of the allocate memory (different when using paging)
void *kmalloc(uint32_t size, uint32_t align, uint32_t *phys){

   heapnode_t* head = (heapnode_t*)kernel_start_heap;

   //When aligning memory, we first act as if no alignment is needed
   //by allocating the size plus the alignment value. We then align
   //the pointer returned within this block. Since we allocated the size
   //plus the alignment in bytes, we can be assured that the alignment
   //will not cause problems.
   uint32_t alloc_size = align ? size + align : size;

   //Go through each block while the current block is allocated or
   //is not large enough
   while( head->allocated || (head->size < alloc_size && head->next_node)){
        head = head->next_node;
   }

   //Make sure we are dealing with a valid chunk of free memory
   if( ! (head->allocated || head->size < alloc_size ) ){

      //It only makes sense to split off the free portion of memory
      //if it is large enough. This is set by MIN_SPLIT. In practice,
      //we should never really run out of space
      if( head->size - alloc_size - sizeof(heapnode_t) > MIN_SPLIT ){

         heapnode_t *current_node = head;
         heapnode_t *next_item = (heapnode_t*)( (uint8_t*)head + sizeof(heapnode_t) + alloc_size);
      
         next_item->next_node = current_node->next_node;
         next_item->free_mem = (uint8_t*)next_item + sizeof(heapnode_t);
         next_item->size = current_node->size - sizeof(heapnode_t) - alloc_size;
         next_item->allocated = 0;
 
         current_node->next_node = next_item;
         current_node->size = alloc_size;

         if( align ){
            //We overestimate the amount of memory we need when aligning
            uint32_t *start_addr = (uint32_t*)((int8_t*)current_node + sizeof(heapnode_t) + align);

            //Mask the address by taking the 1's complement of the
            //alignment which forms a mask
            current_node->free_mem = (uint32_t*)((uint32_t)start_addr & -align);
         }else{
            current_node->free_mem = ( (int8_t*)current_node + sizeof(heapnode_t) );
         }

         current_node->allocated = 1;

         if( phys )
            *phys = (int)current_node->free_mem;

         return current_node->free_mem;

     }else{
         //We do not have enough left over to make it worth while
         //the split apart the memory chunk

         if( align ){
            //We overestimate the amount of memory we need when aligning
            uint32_t *start_addr = (uint32_t*)((int8_t*)head + sizeof(heapnode_t) + align);

            //Mask the address by taking the 1's complement of the
            //alignment which forms a mask
            head->free_mem = (uint32_t*)((uint32_t)start_addr & -align);
         }else{
            head->free_mem = ( (int8_t*)head + sizeof(heapnode_t) );
         }


         head->allocated = 1;

         if( phys )
            *phys = (int)head->free_mem;
 
         return head->free_mem;
      }
   }
  
   //If we get here, we must have run out of memory.
   return NULL;
}


//Run through the heap and merge as much
//free memory as possible. 
static void unify_heap(){
   heapnode_t *head = (heapnode_t*)kernel_start_heap;

   while( head != NULL ){
      //If we come across two ajacent free memory segments, unify them.
      //Do NOT advance afterwards. Imagine we had three consecutive blocks
      //of dynamic memory 1 2 and 3. On the first pass, we would unify 1 and
      //2 and end up with 1 and 3. We would not want to advance until there
      //is not another block to unify after 1.
      if( head->allocated == 0 && head->next_node->allocated == 0) {

         k_printf("Unifying blocks %x and %x\n", (int)head->free_mem, (int)head->next_node->free_mem);

         head->size += head->next_node->size + sizeof(heapnode_t);
 
         head->next_node = head->next_node->next_node;

     }else{
         //Finally, jump to the next node and repeat.
         head = head->next_node;
      }
   }
}

//Free an allocated chunk of memory
//Note that any address within an allocated block can be
//used to free that block. This is primarily done since alignment
//on a certain boundary using kmalloc will return a pointer that may
//not be the first free memory location
void kfree(void *memChunk){

   heapnode_t *head = (heapnode_t*)kernel_start_heap;
   char found_mem = 0;

   while( head->next_node && ! found_mem){

      //See if the memChunk address falls in the free 
      //space of the current chunk. If it does, we deallocate
      //it and break out of the loop by setting foundMem.
      uint32_t chunkStart = (uint32_t)((uint8_t*)head + sizeof(heapnode_t));
      uint32_t chunkEnd = (uint32_t)((uint8_t*)head + sizeof(heapnode_t) + head->size);

      if( (int)memChunk <= chunkEnd && (int)memChunk >= chunkStart){
         //If we found the chunk, all we need to do it
         //set the allocated flag to 0.
         head->allocated = 0;
         found_mem = 1;
         k_printf("Freed block at %x\n", (int)head->free_mem );
      }else{
         head = head->next_node;
      }
   }

   //Yes, this is very inefficient, 
   //but it is simple.
   unify_heap();
}
