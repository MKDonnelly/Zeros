#include <arch/x86/frame.h>

#include <lib/types.h>
#include <lib/bitwise.h>
#include <lib/string.h>
#include <kernel/mm/heap.h>

//Bitfield for allocated frames (1 for allocated
//0 for free)
static int8_t *allocated_frames;

//Total number of frames being managed
static uint32_t length;

//The start address of the first frame
//This is used as a reference when returning 
//frame addresses
static uint32_t start;

//Given a frame address, deallocate it in the framepool_alloc bitfield 
void framepool_free(uint32_t addr){
    uint32_t index = (addr - start) / ARCH_FRAME_SIZE;

    //Free the frame for use by clearing
    //its allocated bit in framepool_alloc.
    bit_clear( allocated_frames, index );
}

//Specifically allocate a frame. Return -1 if
//the frame is already allocated
int8_t framepool_alloc( uint32_t frame_addr ){
   uint32_t index = (frame_addr - start) / ARCH_FRAME_SIZE;

   if( bit_get( allocated_frames, index ) == 0 ){
      bit_set( allocated_frames, index );
      return 0;
   }

   //Frame was already allocated
   return -1;
}

//Get the allocation status of a frame. 1 = allocated, 0 = free
uint8_t framepool_frame_stat( uint32_t frame_addr ){
   uint32_t index = (frame_addr - start) / ARCH_FRAME_SIZE;
   return bit_get( allocated_frames, index );
}


//Loop over *framepool_alloc to find the first unset bit,
//which will indicate a free frame. Returns 0
//if no frames are present. This returns the physical address
//of the frame if successful.
uint32_t framepool_first_free(){

   //Loop over each bit in framepool_alloc
   for(int offset = 0; offset < length; offset++){
      if( bit_get( allocated_frames, offset ) == 0 ){
         //Found a free frame. Return it and set it as allocated
         bit_set( allocated_frames, offset );
         return (offset * ARCH_FRAME_SIZE) + start;
      }
   }
   //No free frames
   return 0;
}

//Caller specifies the starting address and length
//in frames to manager
void framepool_create(uint32_t paddr_start, uint32_t len){

   //Set the number of frames and starting address
   start = paddr_start;
   //The length will be the number of bits requied. We over-estimate
   //to be safe.
   length = (len / 8)  + 1;

   //Calculate how many bytes need to be allocated
   //for the framepool_alloc bitfield. We over-estimate how
   //many bits we need to be save. At most, we will wast 7 bits.
   allocated_frames = k_malloc( length, 0);
   
   //Zero out the frames bitfield
   memset( allocated_frames, length, 0 );
}
