#include <arch/x86/paging.h>

extern void arch_enable_paging();

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
      //temporary hack. fix later
      return get_frame( get_page_desc( addr, pg_table ) ) | ( addr & 0xFFF );
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


void setup_page( page_desc_t *page, uint8_t is_kernel, uint8_t is_rw, uint32_t frame_addr ){
   //Assume frame address is properly aligned   
   //Only allocate a frame if the page does not have one  
   if( ! page->frame_addr ){
      page->present = 1;
      page->rw = is_rw;
      page->user = is_kernel;
      page->frame_addr = frame_addr / ARCH_FRAME_SIZE;
   }
}

void setup_page_desc( page_desc_t *page, uint8_t is_kernel, uint8_t is_rw, uint32_t frame_addr ){
   //Assume frame address is properly aligned   
   //Only allocate a frame if the page does not have one  
   if( ! page->frame_addr ){
      page->present = 1;
      page->rw = is_rw;
      page->user = is_kernel;
      page->frame_addr = frame_addr / ARCH_FRAME_SIZE;
   }
}

page_directory_t *kernel_page_dir;

void map_page(uint32_t vaddr, uint32_t paddr, page_directory_t *dir){

   //Make sure the table exists before we reference it. If it already
   //exists, this will do nothing.
   make_page_table( vaddr, dir );

   //Get the page table
   page_table_t *page_table = get_page_table( vaddr, dir );

   //Get the page descriptor within the page table
   page_desc_t *page_descriptor = get_page_desc( vaddr, page_table );

   //Map the virtual to physical address
   setup_page_desc( page_descriptor, 1, 1, paddr );
}
 

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
      map_page( i, i, kernel_page_dir );
   }


   //Setup the interrupt handler for paging BEFORE enabling paging
   arch_register_interrupt( PAGE_INTERRUPT, page_int_handler);

   //Let the processor know where our page table is
   //and enable paging.
   load_page_dir( kernel_page_dir );
}

//Copies the contents of one page to another. Both are physical
//addresses.
void copy_page( uint8_t *dest, uint8_t *src ){
   
   //NOTE: In the future, we may have a higher-half kernel.
   //in that case, this would fail (eip is virtual). Instead, we
   //could have a "window" page that we temporarily map here in the
   //kernel page directory to copy over the page. This way, we would
   //never have to disable paging and mess up everything.
   arch_disable_paging();

   for(int i = 0; i < ARCH_PAGE_SIZE; i++){
      dest[i] = src[i];
   }
   
   arch_enable_paging();
}


page_table_desc_t clone_table(page_table_desc_t source_desc){

   //Convert the source page table descriptor 

   page_table_t *src = (page_table_t*)(source_desc.table_addr * ARCH_PAGE_SIZE);

   //Make a new page table
   page_table_t *new_table = (page_table_t*)k_malloc( kernel_heap, sizeof(page_table_t), ARCH_PAGE_SIZE );
   memset(new_table, sizeof(page_table_t), 0);

   //For every page descriptor in the table, make a new one
   //in the copy and copy the data over.
   for(int i = 0; i < PDES_IN_PT; i++){
      //Only create a new page in the copy if the source has one
      if( src->page_descriptors[i].frame_addr ){
         new_table->page_descriptors[i].frame_addr = first_free_frame();
         new_table->page_descriptors[i].rw = src->page_descriptors[i].rw;
         new_table->page_descriptors[i].user = src->page_descriptors[i].user;
         new_table->page_descriptors[i].accessed = src->page_descriptors[i].accessed;
         new_table->page_descriptors[i].dirty = src->page_descriptors[i].dirty;

         //Finally, copy over the data
         copy_page( (uint8_t*)(src->page_descriptors[i].frame_addr * ARCH_PAGE_SIZE),
                    (uint8_t*)(new_table->page_descriptors[i].frame_addr * ARCH_PAGE_SIZE));

      }
   }

   //Now create a page table descriptor referencing the table we made
   page_table_desc_t descriptor;
   descriptor.present = 1;
   descriptor.rw = 1;
   descriptor.user_access = 1;
   descriptor.table_addr = virt_to_phys( (uint32_t)new_table, current_directory ) / ARCH_FRAME_SIZE;

   return descriptor;
}



page_directory_t *clone_page_dir(page_directory_t *copy_from){

   page_directory_t *new_dir = k_malloc( kernel_heap, sizeof(page_directory_t), ARCH_PAGE_SIZE );
   memset( new_dir, sizeof(page_directory_t), 0 );

   for(int i = 0; i < PTDES_IN_PD; i++){
      //All page directories must ultimatelly inherit from kernel_page_dir
      //since when we switch tasks, the scheduler code MUST be in the
      //same place. This is why we link.      
      if( kernel_page_dir->pt_descriptors[i].table_addr == copy_from->pt_descriptors[i].table_addr ){
         new_dir->pt_descriptors[i] = copy_from->pt_descriptors[i];
      }else{
         //If there is a directory in copy_from that is not in the 
         //kernel page directory, make a copy of it.
         new_dir->pt_descriptors[i] = clone_table( copy_from->pt_descriptors[i] );
         new_dir->pt_descriptors[i].present = 1;
         new_dir->pt_descriptors[i].rw = 1;
         //Temporary hack
         new_dir->pt_descriptors[i].user_access = 1;
      }
   }
   return new_dir;
}


void page_int_handler(registers_t r){
   int fault_addr;
   //TODO convert to assembly
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

//Copies a buffer to a physical address in memory
//Used when loading in userland processes
void copy_to_physical(char *buf, int amount, uint32_t addr){

   //Get the physical address of the buffer. This is needed since
   //we deal with physical, not virtual, addresses when copying.
   uint32_t buf_physical = virt_to_phys( (uint32_t)buf, kernel_page_dir );

   //Disable paging and interrupts
   arch_disable_ints();
   arch_disable_paging();

   //Copy over buffer
   memcpy( (char*)addr, (char*)buf_physical, amount );

   //Re-enable paging
   arch_enable_paging();
   arch_enable_ints();
}

