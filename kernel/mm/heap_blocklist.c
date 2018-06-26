#include <kernel/mm/heap_blocklist.h>

heap_algs_t blocklist_heap = {
   .malloc = blocklist_malloc,
   .free   = blocklist_free,
   .init_heap = blocklist_init_heap
};

void blocklist_init_heap(heap_t *heap_descriptor){

    heap_block_t* head = (heap_block_t*)heap_descriptor->start;

    //Initilize the first element by setting its size to
    //the amount of memory, the next node to 0, and 
    //is allocated to 0.
    head->next_block = NULL; //No next node, this is the first 
    head->size = heap_descriptor->len - sizeof(heap_block_t);
    heap_descriptor->size_left -= sizeof(heap_block_t);
    head->free_mem = ( (char*)head + sizeof(heap_block_t) );
    head->allocated = 0;

    spinlock_init(&heap_descriptor->heap_lock);
}


//Allocate "size" piece of memory on the heap. If align is non-zero
//align the memory to it's size. If phys is non-null, return the physical
//address of the allocate memory (different when using paging)
void *blocklist_malloc(heap_t *heap_descriptor, uint32_t size, 
                       uint32_t align){

   //Lock the heap to prevent corruption
   spinlock_acquire( &heap_descriptor->heap_lock );

   heap_block_t* head = (heap_block_t*)heap_descriptor->start;

   //When aligning memory, we first act as if no alignment is needed
   //by allocating the size plus the alignment value. We then align
   //the pointer returned within this block. Since we allocated the size
   //plus the alignment in bytes, we can be assured that the alignment
   //will not cause problems.
   uint32_t alloc_size = align ? size + align : size;


   //Go through each block while the current block is allocated or
   //is not large enough
   while(head->allocated || (head->size < alloc_size && head->next_block))
        head = head->next_block;

   //Make sure we are dealing with a valid chunk of free memory
   if( ! (head->allocated || head->size < alloc_size ) ){

      //It only makes sense to split off the free portion of memory
      //if it is large enough. This is set by MIN_SPLIT. In practice,
      //we should never really run out of space
      if( head->size - alloc_size - sizeof(heap_block_t) > MIN_SPLIT ){

         heap_block_t *current_node = head;
         heap_block_t *next_item = (heap_block_t*)( (char*)head + 
                                    sizeof(heap_block_t) + alloc_size);
      
         next_item->next_block = current_node->next_block;
         next_item->free_mem = (char*)next_item + sizeof(heap_block_t);
         next_item->size = current_node->size - sizeof(heap_block_t) - 
                           alloc_size;
         next_item->allocated = 0;
 
         current_node->next_block = next_item;
         current_node->size = alloc_size;

         if( align ){
            //We overestimate the amount of memory we need when aligning
            uint32_t *start_addr = (uint32_t*)((int8_t*)current_node + sizeof(heap_block_t) + align);

            //Mask the address by taking the 1's complement of the
            //alignment which forms a mask
            current_node->free_mem = (uint32_t*)((uint32_t)start_addr & -align);
         }else{
            current_node->free_mem = ( (int8_t*)current_node + sizeof(heap_block_t) );
         }

         current_node->allocated = 1;

         //Unlock the heap
         spinlock_release( &heap_descriptor->heap_lock );
         return current_node->free_mem;

     }else{
         //We do not have enough left over to make it worth while
         //the split apart the memory chunk

         if( align ){
            //We overestimate the amount of memory we need when aligning
            uint32_t *start_addr = (uint32_t*)((int8_t*)head + sizeof(heap_block_t) + align);

            //Mask the address by taking the 1's complement of the
            //alignment which forms a mask
            head->free_mem = (uint32_t*)((uint32_t)start_addr & -align);
         }else{
            head->free_mem = ( (int8_t*)head + sizeof(heap_block_t) );
         }

         head->allocated = 1;

         //Unlock the heap
         spinlock_release( &heap_descriptor->heap_lock );
         return head->free_mem;
      }
   }
  
   //If we get here, we must have run out of memory.
   spinlock_release( &heap_descriptor->heap_lock );
   return NULL;
}


//Run through the heap and merge as much
//free memory as possible. THIS SHOULD ONLY BE CALLED
//BY FREE, SO NO LOCK IS NEEDED!
static void blocklist_unify_heap(heap_t *heap_descriptor){

   heap_block_t *head = (heap_block_t*)heap_descriptor->start;

   while( head != NULL ){
      //If we come across two ajacent free memory segments, unify them.
      //Do NOT advance afterwards. Imagine we had three consecutive blocks
      //of dynamic memory 1 2 and 3. On the first pass, we would unify 1 and
      //2 and end up with 1 and 3. We would not want to advance until there
      //is not another block to unify after 1.
      if( head->allocated == 0 && head->next_block->allocated == 0) {

         head->size += head->next_block->size + sizeof(heap_block_t);
 
         head->next_block = head->next_block->next_block;

     }else{
         //Finally, jump to the next node and repeat.
         head = head->next_block;
      }
   }
}

//Free an allocated chunk of memory
//Note that any address within an allocated block can be
//used to free that block. This is primarily done since alignment
//on a certain boundary using kmalloc will return a pointer that may
//not be the first free memory location
void blocklist_free(heap_t *heap_descriptor, void *memChunk){

   //Lock the heap to prevent corruption
   spinlock_acquire( &heap_descriptor->heap_lock );

   heap_block_t *head = (heap_block_t*)heap_descriptor->start;
   char found_mem = 0;

   while( head->next_block && ! found_mem){

      //See if the memChunk address falls in the free 
      //space of the current chunk. If it does, we deallocate
      //it and break out of the loop by setting foundMem.
      uint32_t chunkStart = (uint32_t)((uint8_t*)head + sizeof(heap_block_t));
      uint32_t chunkEnd = (uint32_t)((uint8_t*)head + sizeof(heap_block_t) + head->size);

      if( (int)memChunk <= chunkEnd && (int)memChunk >= chunkStart){
         //If we found the chunk, all we need to do it
         //set the allocated flag to 0.
         head->allocated = 0;
         found_mem = 1;
      }else{
         head = head->next_block;
      }
   }

   //Yes, this is very inefficient, 
   //but it is simple. We already called lock,
   //so this function will NOT lock the heap again.
   blocklist_unify_heap(heap_descriptor);
   spinlock_release( &heap_descriptor->heap_lock );
}
