#include <arch/x86/frame.h>

//Bitfield for allocated frames (1 for allocated
//0 for free)
int8_t *frames;

//Total number of frames being managed
uint32_t total_frames;

int get_frame_stat( uint32_t frame_addr ){
   return bit_get( &frames, frame_addr / ARCH_FRAME_SIZE );
}

//Specifically allocate a frame. Return -1 if
//the frame is already allocated
int allocate_frame( uint32_t frame_addr ){
   if( bit_get( frames, frame_addr / ARCH_FRAME_SIZE ) == 0 ){
      bit_set( frames, frame_addr / ARCH_FRAME_SIZE );
      return 0;
   }else{
      return -1;
   }
}

//Loop over *frames to find the first unset bit,
//which will indicate a free frame. Return -1
//if no frames are present. This returns the address
//of the frame.
int32_t first_free_frame(){

   //Loop over each bit in frames
   for(int offset = 0; offset < total_frames; offset++){
      if( bit_get( frames, offset ) == 0 ){
         //Found a free frame! Return it and set it as allocated
         bit_set( frames, offset );
         return offset * ARCH_FRAME_SIZE;
      }
   }

   //No free frames available!
   return -1;
}

//Given a frame address, deallocate it in the frames bitfield 
void free_frame(uint32_t addr){
    //Free the frame for use by clearing
    //its allocated bit in *frames.
    bit_clear( frames, addr / ARCH_FRAME_SIZE );
}

//We will presume that we are managing memory from
//0 to memory_max
void init_frames(uint32_t memory_max){

   //Calculate the total number of frames needed
   total_frames = memory_max / ARCH_FRAME_SIZE;

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
