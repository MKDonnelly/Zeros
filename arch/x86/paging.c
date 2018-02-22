#include <arch/x86/paging.h>

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
page_directory_t *kernel_page_dir;

//Loop over *frames to find the first unset bit,
//which will indicate a free frame. Return -1
//if no frames are present.
static int32_t first_free_frame(){
   unsigned int index;
   uint8_t offset;

   //Loop over each char in *frames
   for(index = 0; index < framesSize; index++){

      //Loop over each bit in each element of *frames
      //for( offset = 0; offset < sizeof(char)*8; offset++){
      for( offset = 0; offset < CHAR_BITS; offset++){
         if( ! bitGet( &frames[index], offset ) ){
             //We have found a bit not set, so return
             //the index of the frame for use.
             //return sizeof(char)*8 * index + offset;
             return CHAR_BITS * index + offset;
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
uint8_t page_map(page_entry_t *page, uint8_t is_kernel, uint8_t is_writeable, uint32_t physical){
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
      bitSet( frames, physical / ARCH_FRAME_SIZE );
      page->present = 1; //True, the page is present
      page->rw = is_writeable;
      page->user = is_kernel;
      //the frame member is the frame address, shifted
      //up by 12 bits. To get the physical address in,
      //we will pre-shift the address down 12 bits.
      page->frame = physical / ARCH_FRAME_SIZE;
   }
   return 0;
}


//Allocate a frame. Returns -1 on invalid parameters.
//Returns -2 if there are no available frames.
uint8_t page_map_auto(page_entry_t *page, uint8_t is_kernel, uint8_t is_writeable){

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
void free_frame(page_entry_t *page){

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
   total_frames = mem_end_page / ARCH_PAGE_SIZE;

   //Calculate the size of *frames as the
   //number of chars it needs to hold. Add 1
   //to catch any remainder. Divide by 8 since a
   //char has 8 bits
   framesSize = (total_frames / CHAR_BITS) + 1;

   //Allocate space for the bitset
   frames = (char*)kmalloc( framesSize, 0, 0 );

   //Zero out the frame allocation table.
   memset( frames, framesSize, 0);

   //Make the page directory for the kernel
   kernel_page_dir = (page_directory_t*)kmalloc(sizeof(page_directory_t), ARCH_PAGE_SIZE, 0);

   //Zero out the page directory
   memset( kernel_page_dir, sizeof(page_directory_t), 0);

   //Allocate the lower part of memory for the kernel
   int i = 0;
   while( i < kernel_end_heap){
      //Identity map each page upto the end of the heap.
      page_map(get_page(i, 1, kernel_page_dir), KERNEL_MEMORY, IS_WRITEABLE, i);
      i += ARCH_FRAME_SIZE;
   }

   //Let the processor know where our page table is
   //and enable paging.
   load_page_dir( kernel_page_dir );

   //Finally, setup the interrupt handler
   arch_register_interrupt( PAGE_INTERRUPT, page_int_handler);
}

//Load the given page_directory into cr3
void load_page_dir(page_directory_t *dir){
   asm volatile("mov %0, %%cr3" :: "r"(&dir->tablesPhysical));
   unsigned int cr0;
   asm volatile("mov %%cr0, %0" : "=r"(cr0));
   cr0 |= 0x80000000; //Enable paging
   asm volatile("mov %0, %%cr0" :: "r"(cr0));
}


//Get a pointer to a page. given its physical address
page_entry_t *get_page(uint32_t address, uint8_t make, page_directory_t *dir){
   //Turn the address into an index
   address /= ARCH_FRAME_SIZE;
   //Find the page table containing this address
   unsigned int table_idx = address / 1024;
   if( dir->tables[table_idx] ){
      return &dir->tables[table_idx]->pages[address%1024];
   }else if(make){
      unsigned int tmp;
      dir->tables[table_idx] = (page_table_t*)kmalloc(sizeof(page_table_t), ARCH_PAGE_SIZE, &tmp);
      memset(dir->tables[table_idx], 0x1000, 0);
      dir->tablesPhysical[table_idx] = tmp | 0x7;
      return &dir->tables[table_idx]->pages[address%1024];
   }else{
      return 0;
   }
}

void page_int_handler(registers_t r){
   int fault_addr;
   asm volatile("mov %%cr2, %0" : "=r" (fault_addr));

   uint8_t present = !(r.error & PAGE_PRESENT_M);
   uint8_t rw = r.error & PAGE_RW_M;
   uint8_t us = r.error & PAGE_USR_M;
   uint8_t reserved = r.error & PAGE_RES_M;
   uint8_t id = r.error & PAGE_ID_M;

   k_printf("Page Fault: ");
   if( present )
      k_printf("page not present at ");
   else if(rw)
      k_printf("page read-only at ");
   else if(us)
      k_printf("page ring-0 only at ");
   else if(reserved)
      k_printf("reserved bits overwritten at ");
   else if( id )
      k_printf("instruction fetch at ");
   k_printf("0x%x\n", fault_addr);

   //We currently cannot handle a fault, so
   //halt the machine
   arch_stop_cpu();
}
