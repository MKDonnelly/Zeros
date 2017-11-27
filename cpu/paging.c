#include "paging.h"

//These two global variables are used
//to keep track of how the frames have been
//allocated. nframes will keep track of how many
//frames there are (i.e. how many 4k blocks of 
//memory are available to map using paging).
//*frames is a bitmap to determine if the page
//is being used or not. Note that one char 
//equals 8 bits, so one char element
//in the *frames array can manage 8 physical frames.

int8_t *frames;
uint32_t framesSize; //The number of chars 
                     //we are using in *frames to 
	             //keep track of the frames

uint32_t total_frames; //Number of total frames
                       //available for use

//We will need the end of the heap to determine
//where to stop allocating kernel pages. It is assumed
//that the heap is the highest point in kernel memory. 
extern uint32_t kernel_end_heap;

//The page table which will be visible to
//the kernel. 
page_directory *kernel_page_dir;

//Loop over *frames to find the first unset bit,
//which will indicate a free frame. Return -1
//if no frames are present.
int32_t first_free_frame(){
   unsigned int index;
   uint8_t offset;

   //Loop over each char in *frames
   for(index = 0; index < framesSize; index++){

      //Loop over each bit in each element of *frames
      for( offset = 0; offset < sizeof(char)*8; offset++){
         if( ! bitGet( &frames[index], offset ) ){
             //We have found a bit not set, so return
             //the index of the frame for use.
             return sizeof(char)*8 * index + offset;
         }
      }
   }

   //No free frames available!
   return -1;
}

//Let the caller specify the mapping between physical
//and virtual addresses. Contrast this to page_map_auto
//which allocates the first free frame with the given page_entry
//is_kernel: 0 for kernel memory, 1 for user
//is_wirteable: 1 for writeable, 0 for read-only
uint8_t page_map(page_entry *page, uint8_t is_kernel, uint8_t is_writeable, uint32_t physical){
   //We will assume that the parameters are valid.

   //Make sure the physical address we are given
   //is aligned to 4K. Note that we do not need to
   //make sure the frame has already been allocated;
   //we could implement shared memory later.
   physical &= 0xFFFFF000;

   //Only allocate a frame if the page 
   //does not have one allocated.
   if( ! page->frame ){
      //Initilize the frame
      //Set it as present in our bitmask
      //We receive the physical address, not the offset
      //in the frames pointer, so change it to an index
      //by dividing by FRAME_SIZE
      bitSet( frames, physical / FRAME_SIZE );
      page->present = 1; //True, the page is present
      page->rw = is_writeable;
      page->user = is_kernel;
      //the frame member is the frame address, shifted
      //up by 12 bits. To get the physical address in,
      //we will pre-shift the address down 12 bits.
      page->frame = physical / FRAME_SIZE;
   }
   return 0;
}


//Allocate a frame. Returns -1 on invalid parameters.
//Returns -2 if there are no available frames.
uint8_t page_map_auto(page_entry *page, uint8_t is_kernel, uint8_t is_writeable){

   //We will assume that the parameters are valid

   //Only allocate a frame if the page 
   //does not have one allocated.
   if( ! page->frame ){
      unsigned int firstFreeFrame = first_free_frame();
      if( firstFreeFrame == -1 )
	      return -2;

      //Initilize the frame
      //Set it as present in our bitmask
      bitSet( frames, firstFreeFrame );
      page->present = 1;
      page->rw = is_writeable;
      page->user = is_kernel;
      page->frame = firstFreeFrame;
   }
   return 0;
}


//Given a page, decouple it from the page
void free_frame(page_entry *page){

   //If the page never had a 
   //frame allocated to it...
   if( ! page->frame ){
      return; //Then we shouldn't be here
   }else{
      //Free the frame for use by clearing
      //its allocated bit in *frames.
      bitClear( frames, page->frame );
      page->frame = 0x0; //Zero out the frame member
                         //to be neat.
   }
}


void init_paging(){

   //Assume we have only 16MB of available memory
   //We will extend this later to use the BIOS RAM size
   //returned when booting.
   unsigned int mem_end_page = 0x1000000;

   //Calculate the number of frames in total
   total_frames = mem_end_page / PAGE_SIZE;

   //Calculate the size of *frames as the
   //number of chars it needs to hold. Add 1
   //to catch any remainder.
   framesSize = total_frames / sizeof(char)*8 + 1;

   //Allocate space for the bitset
   frames = (char*)kmalloc( framesSize, 0, 0 );

   //Zero out the frame allocation table.
   memset( frames, framesSize, 0);

   //Make the page directory for the kernel
   kernel_page_dir = (page_directory*)kmalloc(sizeof(page_directory), 1, 0);
   //Zero out the page directory
   memset( kernel_page_dir, sizeof(page_directory), 0);

   //Allocate the lower part of memory for the kernel
   int i = 0;
   while( i < kernel_end_heap){
      //Identity map each page upto the end of the heap.
      page_map(get_page(i, 1, kernel_page_dir), KERNEL_MEMORY, IS_WRITEABLE, i);
      i += FRAME_SIZE;
   }

   //Let the processor know where our page table is
   //and enable paging.
   load_page_dir( kernel_page_dir );

   //Finally, setup the interrupt handler
   register_interrupt( PAGE_INTERRUPT, page_int_handler);
}

//Load the given page_directory into cr3
void load_page_dir(page_directory *dir){
   asm volatile("mov %0, %%cr3" :: "r"(&dir->tablesPhysical));
   unsigned int cr0;
   asm volatile("mov %%cr0, %0" : "=r"(cr0));
   cr0 |= 0x80000000; //Enable paging
   asm volatile("mov %0, %%cr0" :: "r"(cr0));
}


//Get a pointer to a page. given its physical address
page_entry *get_page(uint32_t address, uint8_t make, page_directory *dir){
   //Turn the address into an index
   address /= FRAME_SIZE;
   //Find the page table containing this address
   unsigned int table_idx = address / 1024;
   if( dir->tables[table_idx] ){
      return &dir->tables[table_idx]->pages[address%1024];
   }else if(make){
      unsigned int tmp;
      dir->tables[table_idx] = (page_table*)kmalloc(sizeof(page_table), 1, &tmp);
      memset(dir->tables[table_idx], 0x1000, 0);
      dir->tablesPhysical[table_idx] = tmp | 0x7;
      return &dir->tables[table_idx]->pages[address%1024];
   }else{
      return 0;
   }
}

void page_int_handler(struct registers r){
   int fault_addr;
   asm volatile("mov %%cr2, %0" : "=r" (fault_addr));

   uint8_t present = !(r.error & 0x1);
   uint8_t rw = r.error & 0x2;
   uint8_t us = r.error & 0x4;
   uint8_t reserved = r.error & 0x8;
   uint8_t id = r.error & 0x10;

   k_print("Page Fault: ");
   if( present )
      k_print("page not present at ");
   else if(rw)
      k_print("page read-only at ");
   else if(us)
      k_print("page ring-0 only at ");
   else if(reserved)
      k_print("reserved bits overwritten at ");
   else if( id )
      k_print("instruction fetch at ");
   k_print( "0x" );
   char buffer[17]; //16 chars + \0
   itoh( fault_addr, buffer );
   k_print( buffer );

   //We currently cannot handle a fault, so
   //halt the machine
   asm("hlt");
}
