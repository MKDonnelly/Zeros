#include "paging.h"

//These two global variables are used
//to keep track of how the frames have been
//allocated. nframes will keep track of how many
//frames there are (i.e. how many 4k blocks of 
//memory are available to map using paging).
//*frames is a bitmap to determine if the page
//is being used or not. Note that one unsigned
//int equals 32 bits, so one unsigned int element
//in the *frames array can manage 32 physical frames.

//TODO change to char* instead
unsigned int *frames;
unsigned int framesSize; //The number of unsigned ints
                         //we are using in *frames to 
			 //keep track of the frames
unsigned int nframes;

//This is where the basic heap starts.
//The page table is allocated on the stack
//and we need this to tell the cpu where to
//find the page table.
extern unsigned int start_free_mem;

//The page table which will be visible to
//the kernel.
page_directory *sys_page_table;

//Loop over *frames to find the first unset bit,
//which will indicate a free frame. Return -1
//if no frames are present.
unsigned int first_free_frame(){
   unsigned int index, offset;

   //Loop over each unsigned int in *frames
   for(index = 0; index < framesSize; index++){
      //Loop over each bit in the individual element
      //of *frames
      for(offset = 0; offset < UINT_SIZE; offset++){
         unsigned int mask = (0x1 << offset);
	 if( ! (frames[index] & mask) )
		 return UINT_SIZE * index + offset;
      }
   }
   return -1;
}


//Allocate a frame. Returns -1 on invalid parameters.
//Returns -2 if there are no available frames.
int alloc_frame(page_entry *page, int is_kernel, int is_writeable){

   //Validate parameters
   if( (is_kernel != 0 && is_kernel != 1) ||
       (is_writeable != 0 && is_writeable != 1) )
	   return -1; 

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


void free_frame(page_entry *page){
   unsigned int frame;
   if( !(frame = page->frame) ){
	   return; //The given page didn't actually
                   //have an allocated frame
   }else{
      bitClear( frames, frame );
      page->frame = 0x0;
   }
}


void init_paging(){
   //Assume we have only 16MB of available memory
   unsigned int mem_end_page = 0x1000000;

   //Calculate the number of frames to keep track of
   nframes = mem_end_page / PAGE_SIZE;

   //Calculate the size of *frames as the
   //number of unsigned ints it needs to hold.
   framesSize = nframes / UINT_SIZE;

   frames = (unsigned int*)kmalloc( framesSize, 0, 0 );

   //Zero out the frame allocation table.
   //We need to set the number of bits to set as 
   //framesSize * 4 since each element in frames is
   //an unsigned int (4 bytes)
   memset( (char*)frames, framesSize * 4, 0);

   //Make a page directory
   sys_page_table = (page_directory*)kmalloc(sizeof(page_directory), 1, 0);
   //Zero out the page directory
   memset( (char*)sys_page_table, sizeof(page_directory), 0);

   //Allocate the lower part of memory for the kernel
   int i = 0;
   int count = 0;
   while( i < start_free_mem && count <= 256){
      alloc_frame( get_page(i, 1, sys_page_table), 0, 0);
      i += 0x1000;
      count++;
   }

   //Let the processor know where our page table is
   //and enable paging.
   load_page_dir( sys_page_table );

   //Finally, setup the interrupt handler
   register_interrupt( PAGE_INTERRUPT, page_int_handler);
}

void load_page_dir(page_directory *dir){
   asm volatile("mov %0, %%cr3" :: "r"(&dir->tablesPhysical));
   unsigned int cr0;
   asm volatile("mov %%cr0, %0" : "=r"(cr0));
   cr0 |= 0x80000000; //Enable paging
   asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

page_entry *get_page(unsigned int address, int make, page_directory *dir){
   //Turn the address into an index
   address /= 0x1000;
   //Find the page table containing this address
   unsigned int table_idx = address / 1024;
   if( dir->tables[table_idx] ){
      return &dir->tables[table_idx]->pages[address%1024];
   }else if(make){
      unsigned int tmp;
      dir->tables[table_idx] = (page_table*)kmalloc(sizeof(page_table), 1, &tmp);
      memset((char*)dir->tables[table_idx], 0x1000, 0);
      dir->tablesPhysical[table_idx] = tmp | 0x7;
      return &dir->tables[table_idx]->pages[address%1024];
   }else{
      return 0;
   }
}

void page_int_handler(struct registers r){
   int fault_addr;
   asm volatile("mov %%cr2, %0" : "=r" (fault_addr));

   ubyte present = !(r.error & 0x1);
   ubyte rw = r.error & 0x2;
   ubyte us = r.error & 0x4;
   ubyte reserved = r.error & 0x8;
   ubyte id = r.error & 0x10;

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
