#include "paging.h"



//A bitset of frames
unsigned int *frames;
unsigned int nframes;

extern unsigned int start_free_mem;

//Macros for bitset
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))


void set_frame(unsigned int frame_addr){
   unsigned int frame = frame_addr/0x1000;
   unsigned int idx = INDEX_FROM_BIT(frame);
   unsigned int off = OFFSET_FROM_BIT(frame);
   frames[idx] |= (0x1 << off);
}

//Clear a bit from the frame bitset
static void clear_frame(unsigned int frame_addr){
   unsigned int frame = frame_addr/0x1000;
   unsigned int idx = INDEX_FROM_BIT(frame);
   unsigned int off = OFFSET_FROM_BIT(frame);
   frames[idx] &= ~(0x1 << off);
}

//Test if a bit frame is set
static unsigned int test_frame(unsigned int frame_addr){
   unsigned int frame = frame_addr/0x1000;
   unsigned int idx = INDEX_FROM_BIT(frame);
   unsigned int off = OFFSET_FROM_BIT(frame);
   return (frames[idx] & (0x1 << off) );
}

//Find the first free frame
static unsigned int first_frame(){
   unsigned int i, j;

   for(i = 0; i < INDEX_FROM_BIT(nframes); i++){
      if( frames[i] != 0xFFFFFFFF ){
         for(j = 0; j < 32; j++){
            unsigned int toTest = 0x1 << j;
	    if( !(frames[i] & toTest ) )
              return i*4*8+j;
	 }
      }
   }
   return -1;
}


//Allocate a frame
void alloc_frame(page_t *page, int is_kernel, int is_writeable){
   if( page->frame != 0){
      return;
   }else{
      unsigned int idx = first_frame();
      if( idx == (unsigned int)-1){
         k_print("No free frames!");
         asm("hlt");	 
      }
      set_frame(idx * 0x1000 );
      page->present = 1;
      page->rw = (is_writeable) ? 1 : 0; //Show the page be writeable
      page->user = (is_kernel) ? 0 : 1;  //Should the page be user-mode
      page->frame = idx;
   }
}


void free_frame(page_t *page){
   unsigned int frame;
   if( !(frame = page->frame) ){
	   return; //The given page didn't actually
                   //have an allocated frame
   }else{
      clear_frame(frame);
      page->frame = 0x0;
   }
}


void initialise_paging(){
   //Assume we have only 16MB of available memory
   unsigned int mem_end_page = 0x10000000;

   nframes = mem_end_page / 0x1000;
   frames = (unsigned int*)kmalloc( INDEX_FROM_BIT(nframes), 0, 0 );
   memset( (char*)frames, INDEX_FROM_BIT(nframes), 0);

   //Make a page directory
   page_directory_t *kernel_directory = (page_directory_t*)kmalloc(sizeof(page_directory_t), 1, 0);
   memset( (char*)kernel_directory, sizeof(page_directory_t), 0);
   //current_directory = kernel_directory;

   int i = 0;
   while( i < start_free_mem ){
      alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
      i += 0x1000;
   }

   switch_page_directory( kernel_directory );
}

void switch_page_directory(page_directory_t *dir){
   asm volatile("mov %0, %%cr3" :: "r"(&dir->tablesPhysical));
   unsigned int cr0;
   asm volatile("mov %%cr0, %0" : "=r"(cr0));
   cr0 |= 0x80000000; //Enable paging
   asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

page_t *get_page(unsigned int address, int make, page_directory_t *dir){
   //Turn the address into an index
   address /= 0x1000;
   //Find the page table containing this address
   unsigned int table_idx = address / 1024;
   if( dir->tables[table_idx] ){
      return &dir->tables[table_idx]->pages[address%1024];
   }else if(make){
      unsigned int tmp;
      dir->tables[table_idx] = (page_table_t*)kmalloc(sizeof(page_table_t), 1, &tmp);
      memset((char*)dir->tables[table_idx], 0x1000, 0);
      dir->tablesPhysical[table_idx] = tmp | 0x7;
      return &dir->tables[table_idx]->pages[address%1024];
   }else{
      return 0;
   }
}

