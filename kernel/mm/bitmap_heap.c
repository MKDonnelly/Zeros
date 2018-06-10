#include <kernel/mm/bitmap_heap.h>

#include <lib/string.h>
#include <lib/assert.h>
#include <lib/bitwise.h>

//Operators for a dibit set. A dibit set is a bitfield comprised
//of two-bit values. Think of it as an array of two-bit types. This
//is useful for the bitmap heap since we can "color" each alloction
//with a 0, 1, 2, or 3. We can then differentiate different ajacent
//memory allocations, which means we do not need to store the number
//or size of memory blocks allocated!
//                        Accounting dibit
//--------------------------------------------------------------------
//00000011111111111122222222222211111111111133333333333300000000000000 
//--------------------------------------------------------------------
//                             Memory
//--------------------------------------------------------------------
//<free><allocation><allocation><allocation><allocation><free.......>
//--------------------------------------------------------------------

void dibit_set(void *mem, int index, int val){
   if( val & 0x2 )
      bit_set(mem, index * 2);
   else
      bit_clear(mem, index * 2);

   if( val & 0x1 )
      bit_set(mem, index * 2 + 1);
   else
      bit_clear(mem, index * 2 + 1);
}

int dibit_get(void *mem, size_t idx){
   return (bit_get( mem, idx * 2 ) << 1) | bit_get(mem, idx * 2 + 1);
}


heap_algs_t bitmap_heap = {
   .malloc = bitmap_malloc,
   .free   = bitmap_free,
   .init_heap = bitmap_init_heap
};


void bitmap_init_heap(heap_t *heap){
   bitmap_heap_t *head = (bitmap_heap_t*)(heap->start);

   head->blocksize = DEFAULT_BLOCKSIZE;

   //Add 1 to round up. 
   int total_blocks = heap->len / DEFAULT_BLOCKSIZE + 1;
   //Divide by 8 since there are 8 bits in a char
   head->dibitmap_len = (total_blocks * 2) / 8;
   head->dibitmap = (char*)(heap->start + sizeof(heap_t) + sizeof(bitmap_heap_t));
   memset( head->dibitmap, head->dibitmap_len, 0);

   //Align the start of the heap on a block size for easier alignment
   head->free_space = (char*)ALIGN_ON((size_t)(head->dibitmap + 
                      head->dibitmap_len), head->blocksize);
}

void bitmap_free(heap_t *heap, void *memblock){
   bitmap_heap_t *head = (bitmap_heap_t*)heap->start;

   spinlock_acquire(&heap->heap_lock);

   size_t offset = (size_t)memblock - (size_t)head->free_space;
   int index = offset / head->blocksize;

   int color = dibit_get(head->dibitmap, index);

   //Clear all blocks forward
   int i = 0;
   while( dibit_get( head->dibitmap, index + i) == color ){
      dibit_set( head->dibitmap, index + i, DIBIT_FREE );
      i++;
   }

   //Clear all blocks backwards
   i = 1;
   while( (index - i >= 0) && dibit_get( head->dibitmap, index - i ) == color ){
      dibit_set( head->dibitmap, index - i, DIBIT_FREE );
      i++;
   }

   spinlock_release(&heap->heap_lock);
}


//Given an address, an alignment value, and a blocks size,
//returns true if the address can be aligned to within the block
//size, else returns false.
#define ALIGN_WITHIN( addr, align_size, block_size )\
        ( (ALIGN_ON( addr, align_size ) - addr) < block_size ? 1 : 0 )

void *bitmap_malloc(heap_t *heap, size_t size, size_t align){
   bitmap_heap_t *head = (bitmap_heap_t*)heap->start;
   spinlock_acquire( &heap->heap_lock );

   //Add 1 to catch any fractional part of a block required
   size_t blocks_needed = (size / head->blocksize) + 1;
   
   KASSERT( blocks_needed < (head->dibitmap_len * 4) );

   //Multiply by 4 since there are 4 groups of 2 bits in a byte
   //and dibitmap_len is in bytes.
   for(size_t i = 0; i < head->dibitmap_len * 4; i++){
      int found = 1;
      for(int j = 0; j < blocks_needed; j++){
         if( dibit_get( head->dibitmap, i + j) != 0 )
            found = 0;
         
         if( align != 0 && !ALIGN_WITHIN( (size_t)(head->free_space + 
             i * head->blocksize), align, head->blocksize ) )
            found = 0;
      }
      if( found ){
         //Color the block to ensure that we can deallocate it later
         int left_color;
         if( i != 0 )
            left_color = dibit_get(head->dibitmap, i - 1);
         else
            left_color = 0;

         int right_color = dibit_get(head->dibitmap, i + blocks_needed);

         int color = 0;
         if( DIBIT_BLUE != left_color && DIBIT_BLUE != right_color ){
            color = DIBIT_BLUE;
         }else if( DIBIT_RED != left_color && DIBIT_RED != right_color){
            color = DIBIT_RED;
         }else if( DIBIT_GREEN != left_color && DIBIT_RED != right_color){
            color = DIBIT_GREEN;
         }

         //Color the blocks
         for(size_t j = 0; j < blocks_needed; j++){
            dibit_set( head->dibitmap, i + j, color );
         }

         spinlock_release(&heap->heap_lock);
         return (void*)ALIGN_ON( (size_t)(head->free_space + 
                          i * head->blocksize), head->blocksize);
      }
   }
   spinlock_release( &heap->heap_lock );
   //No memory left.
   return NULL;
}
