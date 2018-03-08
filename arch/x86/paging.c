#include <arch/x86/paging.h>

//The page table which will be visible to
//the kernel. 
page_directory_t *kernel_page_dir;

//Let the caller specify the mapping between physical
//and virtual addresses. 
uint8_t page_map(page_entry_t *page, uint8_t is_kernel, uint8_t is_writeable, uint32_t frame_addr){
   //We will assume that the parameters are valid.

   //Make sure the physical address we are given
   //is aligned to 4K. Note that we do not need to
   //make sure the frame has already been allocated;
   //we could implement shared memory later.
   frame_addr &= 0xFFFFF000;

   //Only allocate a frame if the page 
   //does not have one allocated.
   if( ! page->frame ){

      allocate_frame( frame_addr );

      //Initilize the frame
      //Set it as present in our bitmask
      //We receive the physical address, not the offset
      //in the frames pointer, so change it to an index
      //by dividing by FRAME_SIZE
      page->present = 1; //True, the page is present
      page->rw = is_writeable;
      page->user = is_kernel;
      //FIXME Hack for userland initial support 
      //page->user = 1;
      //the frame member is the frame address, shifted
      //up by 12 bits. To get the physical address in,
      //we will pre-shift the address down 12 bits.
      page->frame = frame_addr / ARCH_FRAME_SIZE;
   }
   return 0;
}


//Load the given page_directory into cr3
//TODO convert to assembly file
void load_page_dir(page_directory_t *dir){
   asm volatile("mov %0, %%cr3" :: "r"(&dir->tablesPhysical));
   unsigned int cr0;
   asm volatile("mov %%cr0, %0" : "=r"(cr0));
   cr0 |= 0x80000000; //Enable paging
   asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

//Given a page directory, return a pointer to the given page 
//specified by address
page_entry_t *get_page(uint32_t address, uint8_t make, page_directory_t *dir){

   //The page table can be thought of as an array of element
   //of size ARCH_PAGE_SIZE. Turn the address into an offset
   //within this array.
   address /= ARCH_FRAME_SIZE;

   //Even though we have an index, this is not really an array.
   //So from the given index, we must first calculate which page
   //table this address lies and then within the page table, which
   //page entry is correct. 

   //Find the page table containing this address
   unsigned int page_table_idx = address / PT_IN_PD;

   //We can now directly use page_table_idx when accessing
   //the page table array within the page directory

   //If the directory already exists, then the page must exist
   if( dir->tables[ page_table_idx ] ){
      //return the page
      return &dir->tables[ page_table_idx ]->pages[address % PE_IN_PT];
   }else if(make){
      //Else if we request a page to be made, create the page. We actually
      //do not create an individual page entry; we create a page table 
      //which consists of 1024 page entries.

      unsigned int tmp;
      //put the new page table into the directory
      dir->tables[ page_table_idx ] = (page_table_t*)k_malloc( kernel_heap, sizeof(page_table_t), ARCH_PAGE_SIZE, &tmp);

      //Zero out the page table. This creates zero in the place of the
      //page entry, which means it is not present
      memset(dir->tables[ page_table_idx ], TABLE_SIZE, 0);
      dir->tablesPhysical[ page_table_idx ] = tmp | 0x7;

      return &dir->tables[ page_table_idx ]->pages[address % PE_IN_PT];
   }
   return 0;
}

//Given a page, return its frame's address. This effectively turns
//a virtual into a physical address.
uint32_t get_frame( page_entry_t *page ){
   return page->frame * ARCH_FRAME_SIZE;
}

void init_paging(){

   //Assume we have only 16MB of available memory
   //Let the kernel manage memory from 0..5M and
   //leave 5M..16M unmapped for userland processes
   unsigned int mem_end_page =    0x1000000;
   unsigned int kernel_end_page = 0x500000;

   //Initilize frame management system
   init_frames(mem_end_page);

   //Make the page directory for the kernel
   kernel_page_dir = (page_directory_t*)k_malloc( kernel_heap, sizeof(page_directory_t), ARCH_PAGE_SIZE, 0);

   //Zero out the page directory
   memset( kernel_page_dir, sizeof(page_directory_t), 0);

   //Allocate the lower part of memory for the kernel
   int i = 0;

   while( i < kernel_end_page){
      //Identity map each page upto the end of the heap.
      page_map(get_page(i, 1, kernel_page_dir), KERNEL_MEMORY, IS_WRITEABLE, i);
      i += ARCH_FRAME_SIZE;
   }

   //Setup the interrupt handler for paging BEFORE enabling paging
   arch_register_interrupt( PAGE_INTERRUPT, page_int_handler);

   //Let the processor know where our page table is
   //and enable paging.
   load_page_dir( kernel_page_dir );
}


page_table_t *clone_table(page_table_t *src, uint32_t *phys_addr){

   //Make a new page table
   page_table_t *table = (page_table_t*)k_malloc( kernel_heap, sizeof(page_table_t), ARCH_PAGE_SIZE, phys_addr );
   memset(table, sizeof(page_table_t), 0);

   //For every entry in table
   int i;
   for(i = 0; i < 1024; i++){
      if(!src->pages[i].frame)
         continue;
      table->pages[i].frame = first_free_frame();

      //clone the flags
      table->pages[i].present = src->pages[i].present;
      table->pages[i].rw = src->pages[i].rw;
      table->pages[i].user = src->pages[i].user;
      table->pages[i].accessed = src->pages[i].accessed;
      table->pages[i].dirty = src->pages[i].dirty;

      //Copy over the data
      copy_page_physical(src->pages[i].frame*0x1000, table->pages[i].frame*0x1000);
   }
   return table;
}


page_directory_t *clone_dir(page_directory_t *src){

   uint32_t phys;

   page_directory_t *dir = (page_directory_t*)k_malloc( kernel_heap, sizeof(page_directory_t), ARCH_PAGE_SIZE, &phys );
   //zero out new directory
   memset( dir, sizeof(page_directory_t), 0);
   
   uint32_t offset = (uint32_t)dir->tablesPhysical - (uint32_t)dir;
   dir->physicalAddr = phys + offset;


   //Clone page directory from src 
   for(int i = 0; i < PT_IN_PD; i++){
      //Make sure table is present
      if( !src->tables[i] )
         continue;

      if( kernel_page_dir->tables[i] == src->tables[i] ){
         dir->tables[i] = src->tables[i];
         dir->tablesPhysical[i] = src->tablesPhysical[i];
      }else{
         uint32_t phys;
         dir->tables[i] = clone_table(src->tables[i], &phys);
         dir->tablesPhysical[i] = phys | 0x07;
      }
   }
   return dir;
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



