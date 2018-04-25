#include <arch/x86/frame.h>

//Bitfield for allocated frames (1 for allocated
//0 for free)
static int8_t *framepool_alloc;

//Total number of frames being managed
static uint32_t framepool_length;

//The start address of the first frame
//This is used as a reference when returning 
//frame addresses
static uint32_t framepool_start;

//Given a frame address, deallocate it in the framepool_alloc bitfield 
void free_frame(uint32_t addr){
    uint32_t index = (addr - framepool_start) / ARCH_FRAME_SIZE;

    //Free the frame for use by clearing
    //its allocated bit in framepool_alloc.
    bit_clear( framepool_alloc, index );
}

//Specifically allocate a frame. Return -1 if
//the frame is already allocated
int8_t allocate_frame( uint32_t frame_addr ){
   uint32_t index = (frame_addr - framepool_start) / ARCH_FRAME_SIZE;

   if( bit_get( framepool_alloc, index ) == 0 ){
      bit_set( framepool_alloc, index );
      return 0;
   }

   //Frame was already allocated
   return -1;
}

//Get the allocation status of a frame. 1 = allocated, 0 = free
uint8_t get_frame_stat( uint32_t frame_addr ){
   uint32_t index = (frame_addr - framepool_start) / ARCH_FRAME_SIZE;
   return bit_get( framepool_alloc, index );
}


//Loop over *framepool_alloc to find the first unset bit,
//which will indicate a free frame. Returns 0
//if no frames are present. This returns the physical address
//of the frame if successful.
uint32_t first_free_frame(){

   //Loop over each bit in framepool_alloc
   for(int offset = 0; offset < framepool_length; offset++){
      if( bit_get( framepool_alloc, offset ) == 0 ){
         //Found a free frame. Return it and set it as allocated
         bit_set( framepool_alloc, offset );
         return (offset * ARCH_FRAME_SIZE) + framepool_start;
      }
   }
   //No free frames
   return 0;
}

//Caller specifies the starting address and length
//in frames to manager
void make_frame_pool(uint32_t paddr_start, uint32_t length){

   //Set the number of frames and starting address
   framepool_start = paddr_start;
   //The length will be the number of bits requied. We over-estimate
   //to be safe.
   framepool_length = (length / 8)  + 1;

   //Calculate how many bytes need to be allocated
   //for the framepool_alloc bitfield. We over-estimate how
   //many bits we need to be save. At most, we will wast 7 bits.
   framepool_alloc = k_malloc(kernel_heap, framepool_length, 0);
   
   //Zero out the frames bitfield
   memset( framepool_alloc, framepool_length, 0 );
}
