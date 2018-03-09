#include <arch/x86/paging.h>

//Points to current page directory in use
page_directory_t *current_directory = NULL;


//Given a page directory and an address, return a pointer to 
//the page table (the PAGE TABLE, not the PAGE DIR ENTRY). 
page_table_t *get_page_table( uint32_t addr, page_directory_t *dir ){

   //First, get the page directory entry in the page directory
   page_table_desc_t *ptd = &( dir->pt_descriptors[ offset_in_pd( addr ) ]);

   //If the page entry is not null (not used), calculate its address
   //from the table_addr member and return it.
   if( ptd != 0 ){
      return ( (page_table_t*)(ptd->table_addr * ARCH_PAGE_SIZE) );
   }else{
      return 0;
   }
}

page_desc_t *get_page_desc( uint32_t addr, page_table_t *table){   
   return &( table->page_descriptors[ offset_in_pt( addr ) ] );
}


uint32_t virt_to_phys( uint32_t addr, page_directory_t *dir ){

   //This is meant to happen if, before paging is enabled, we
   //pass in current_directory.
   if( dir == NULL )
      return addr;

   //Check to make sure directory exists before we dereference it
   page_table_t *pg_table = get_page_table( addr, dir );
   if(  pg_table != 0 ){
      return get_frame( get_page_desc( addr, pg_table ) );
   }else{
      //Page not mapped, return 0
      return 0;
   }
}

//Create a page table for the given virtual address.
void make_page_table(uint32_t addr, page_directory_t *dir ){
   uint32_t pd_offset = offset_in_pd( addr );
   if( dir->pt_descriptors[pd_offset].table_addr == 0 ){

      //Create a new page table to use
      page_table_t *new_table = k_malloc( kernel_heap, sizeof(page_table_t), ARCH_PAGE_SIZE );

      //We need to place the physical address of the allocated block
      //of memory into the descriptor. So take the virtual address
      //along with the current directory in effect and find physical.
      int phys = virt_to_phys( (uint32_t)new_table, current_directory );

      dir->pt_descriptors[pd_offset].table_addr = phys / ARCH_FRAME_SIZE;
      dir->pt_descriptors[pd_offset].present = 1;
      dir->pt_descriptors[pd_offset].rw      = 1;
      dir->pt_descriptors[pd_offset].user_access = 1;
   }
}

//Given a page, return its frame's address. This effectively turns
//a virtual into a physical address.
uint32_t get_frame( page_desc_t *page ){
   return page->frame_addr * ARCH_FRAME_SIZE;
}


//Load the given page_directory into cr3
//TODO convert to assembly file
void load_page_dir(page_directory_t *dir){
   asm volatile("mov %0, %%cr3" :: "r"(dir->pt_descriptors) );
   unsigned int cr0;
   asm volatile("mov %%cr0, %0" : "=r"(cr0));
   cr0 |= 0x80000000; //Enable paging
   asm volatile("mov %0, %%cr0" :: "r"(cr0));
}


void map_page( page_desc_t *page, uint8_t is_kernel, uint8_t is_rw, uint32_t frame_addr ){
   //Assume frame address is properly aligned   
   //Only allocate a frame if the page does not have one  
   if( ! page->frame_addr ){
      page->present = 1;
      page->rw = is_rw;
      page->frame_addr = frame_addr / ARCH_FRAME_SIZE;
   }
}


///////////////////////////////////////////////////////////
page_directory_t *kernel_page_dir;


void init_paging(){

   //Assume we have only 16MB of available memory
   //Let the kernel manage memory from 0..5M and
   //leave 5M..16M unmapped for userland processes
   //unsigned int mem_end_page =    0x1000000;
   unsigned int kernel_end_page = 0x500000;

   //Make the page directory for the kernel
   kernel_page_dir = (page_directory_t*)k_malloc( kernel_heap, sizeof(page_directory_t), ARCH_PAGE_SIZE );
   memset( kernel_page_dir, sizeof(page_directory_t), 0 );

   for(int i = 0; i < kernel_end_page; i += 0x1000 ){
      //Get the page table and create it.
      make_page_table( i, kernel_page_dir );
      page_table_t *pt = get_page_table( i, kernel_page_dir );
      page_desc_t *pe = get_page_desc( i, pt );
      map_page( pe, 1, 1, i );
   }

   //Setup the interrupt handler for paging BEFORE enabling paging
   arch_register_interrupt( PAGE_INTERRUPT, page_int_handler);

   //Let the processor know where our page table is
   //and enable paging.
   load_page_dir( kernel_page_dir );
}

/*
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
}*/


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
