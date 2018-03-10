#include <arch/x86/frame.h>

//Bitfield for allocated frames (1 for allocated
//0 for free)
int8_t *frames;

//Total number of frames being managed
uint32_t total_frames;

//The start address of the first frame
//This is used as a reference when returning 
//frame addresses
uint32_t first_frame_addr;

int get_frame_stat( uint32_t frame_addr ){
   uint32_t index = (frame_addr - first_frame_addr) / ARCH_FRAME_SIZE;
   return bit_get( frames, index );
}

//Specifically allocate a frame. Return -1 if
//the frame is already allocated
/*
int allocate_frame( uint32_t frame_addr ){
   if( bit_get( frames, frame_addr / ARCH_FRAME_SIZE ) == 0 ){
      bit_set( frames, frame_addr / ARCH_FRAME_SIZE );
      return 0;
   }else{
      return -1;
   }
}*/


//Loop over *frames to find the first unset bit,
//which will indicate a free frame. Return -1
//if no frames are present. This returns the address
//of the frame.
uint32_t first_free_frame(){

   //Loop over each bit in frames
   for(int offset = 0; offset < total_frames; offset++){
      if( bit_get( frames, offset ) == 0 ){
         //Found a free frame! Return it and set it as allocated
         bit_set( frames, offset );
         return (offset * ARCH_FRAME_SIZE) + first_frame_addr;
      }
   }
   //No free frames!
   return 0;
}

//Given a frame address, deallocate it in the frames bitfield 
/*void free_frame(uint32_t addr){
    //Free the frame for use by clearing
    //its allocated bit in *frames.
    bit_clear( frames, addr / ARCH_FRAME_SIZE );
}*/

//Caller specifies the starting address and length
//in frames to manager
void init_frames(uint32_t start_addr, uint32_t frames_len){

   //Set the number of frames and starting address
   total_frames = frames_len;
   first_frame_addr = start_addr;

   //Calculate how many bytes need to be allocated
   //for the frames bitfield. We over-estimate how
   //many bytes are needed (i.e. if we need 17 pages,
   //we allocate 3 bytes)
   int frames_length = (total_frames / 8) + 1;

   //Allocate memory for the bitfield
   frames = (char*)k_malloc(kernel_heap, frames_length, 0);

   //Zero out the frames bitfield
   memset( frames, frames_length, 0 );
}
