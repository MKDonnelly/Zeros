#include <arch/x86/pmode/paging.h>

#include <arch/x86/drivers/vgacommon/vgacommon.h>
#include <arch/x86/pmode/isr.h>
#include <arch/x86/frame.h>
#include <arch/x86/pmode/pagingasm.h>
#include <arch/x86/cpu.h>
#include <arch/x86/pmode/context.h>
#include <kernel/mm/heap.h>

#include <lib/string.h>
#include <lib/types.h>
#include <lib/print.h>
#include <lib/assert.h>

//Points to current page directory in use as well
//as the page directory used by the kernel.
pd_t *kernel_page_dir;

//Given a page directory and a virtual address, return the 
//pte corresponding to it. If create_pt is set, create any
//page tables needed. If create_pt is not set and the pt
//is not allocated, return null
static pte_t *get_page( uint32_t vaddr, bool create_pt, pd_t *page_directory){

   //Get the page table
   pde_t *page_dir_entry = &page_directory->pd_entries[ PD_INDEX( vaddr ) ];
   //Convert the page directory entry into the table it describes, creating
   //the page table if needed.
   if( page_dir_entry->table_addr == 0 && create_pt == 0 ){
      //Page table does not exist and we have not been asked to make one.
      //Return null as an error.
      return NULL;
   }else if( page_dir_entry->table_addr == 0 && create_pt ){
      //Create a new page table to use
      pt_t *new_pt = k_malloc( sizeof(pt_t), ARCH_PAGE_SIZE );

      //We need to place the physical address of the allocated block
      //of memory into the descriptor. So take the virtual address
      //along with the current directory in effect and find physical.
      uint32_t phys = (uint32_t)VIRT_TO_PHYS( (uint32_t)new_pt );
      page_dir_entry->table_addr = phys / ARCH_PAGE_SIZE;
      page_dir_entry->present = PAGE_PRESENT;
      page_dir_entry->rw = PAGE_RW;
      page_dir_entry->user_access = PAGE_USR_ACCESS;
   }

   //At this point, the page table is known to exist
   pt_t *page_table = (pt_t*)(PHYS_TO_VIRT(page_dir_entry->table_addr * ARCH_PAGE_SIZE) );

   //Get the page table entry from the page table
   pte_t *page_table_entry = &page_table->pt_entries[ PT_INDEX( vaddr ) ];
   return page_table_entry;
}

uint32_t virt_to_phys( uint32_t uaddr, pd_t *page_directory ){
     pte_t *pte = get_page( uaddr, 0, page_directory );
     if( pte != NULL )
        return pte->frame_addr * ARCH_PAGE_SIZE;
     return 0; //Address not mapped
}

void vm_pmap(uint32_t vaddr, uint32_t paddr, pd_t *page_directory, 
             uint8_t rw, uint8_t user_access){

   //Get the pte to map the page
   pte_t *page = get_page( vaddr, 1, page_directory );

   //Setup the actual mapping
   page->frame_addr = paddr / ARCH_FRAME_SIZE;
   page->present = PAGE_PRESENT;
   page->rw = rw;
   page->user = user_access;
}

//TODO create map_page_range that does not care about physical memory
//location and just uses the frame allocation system.
void vm_pmap_range( uint32_t vaddr, uint32_t paddr, pd_t *page_directory, 
                     uint8_t rw, uint8_t user_access, uint32_t length ){
   while( length > 0 ){
      vm_pmap( vaddr, paddr, page_directory, rw, user_access );
      vaddr += ARCH_PAGE_SIZE;
      paddr += ARCH_PAGE_SIZE;
      length -= ARCH_PAGE_SIZE;
   }
}

//Used for pages that frequently change (like when used by copy_to_physiscal)
void vm_pmap_temp( uint32_t vaddr, uint32_t paddr, pd_t *page_directory){
   vm_pmap( vaddr, paddr, page_directory, PAGE_RW, PAGE_KERNEL_ACCESS );

   //We expect the mapping to change frequently, so this ensure that when
   //we return, there is no cached reference to the page we mapped.
   inval_page( vaddr );
}

//Copies len bytes of memory at the virtual address vbuf to the physical
//address paddr.
void vm_copy_to_physical(char *vbuf, uint32_t paddr, uint32_t len){
   //Since we are using a higher-half kernel, we cannot disable paging.
   //To access the physical memory, we make a window within the current
   //page directory (the kernel page directory) at 0x0 that maps to
   //the desired physical address. If we need to copy more than one page,
   //we increment the physical frame pointed to as we copy.
   //Align page frame on 4K boundary
   vm_pmap_temp( 0x0, ALIGN_4K(paddr), kernel_page_dir );

   //FIXME len is assumed to be less than the page size
   memcpy( (char*)0x0, vbuf, len );
}

//Copies data from a virtual address in a page directory to the given
//buffer
void vm_copy_from_pdir(uint32_t vaddr, pd_t *page_directory, 
                       char *to, uint32_t len){

   uint32_t physical_page_addr = virt_to_phys( vaddr, page_directory );

   //physical_page_addr will only give us the addres of the page, not
   //the actual offset within that page.
   uint32_t offset_into_page = vaddr & 0xFFF;

   uint32_t physical_addr = physical_page_addr + offset_into_page;

   //Map it into the kernel address space
   vm_pmap_temp( 0x0, physical_page_addr, page_directory );

   //Note: currently we assume that the data remains within a page
   KASSERT( len < ARCH_PAGE_SIZE );
   memcpy( to, offset_into_page, len );
}

//Copies the page at one physical address to another 
//We map the page 0x0 to pdest and 0x1000 to psrc
static void copy_page( uint32_t pdest, uint32_t psrc ){
   vm_pmap_temp( 0x0, pdest, kernel_page_dir );
   vm_pmap_temp( 0x1000, psrc, kernel_page_dir );
   
   memcpy( (char*)0x0, (char*)0x1000, ARCH_PAGE_SIZE );

   //No need to unmap the pages since we called using quick_map
}

static void clone_table(pde_t *dest_pde, pde_t *source_pde){

   //Convert each into a pointer to the page table
   if( source_pde->table_addr == 0 )
      return;

   pt_t *dest_pt = (pt_t*)PHYS_TO_VIRT(dest_pde->table_addr * ARCH_PAGE_SIZE);
   pt_t *source_pt = (pt_t*)k_malloc( sizeof(pt_t), ARCH_PAGE_SIZE);
   memset(source_pt, sizeof(pt_t), 0);

   //For every page descriptor in the source table, make a new page
   //descriptor in the destination and copy the data
   for(int i = 0; i < PTE_IN_PT; i++){

      //Only create a new page in the copy if the source has one
      if( source_pt->pt_entries[i].frame_addr != 0 ){
         dest_pt->pt_entries[i].frame_addr = framepool_first_free();
         dest_pt->pt_entries[i].rw = source_pt->pt_entries[i].rw;
         dest_pt->pt_entries[i].user = source_pt->pt_entries[i].user;
         dest_pt->pt_entries[i].accessed = source_pt->pt_entries[i].accessed;
         dest_pt->pt_entries[i].dirty = source_pt->pt_entries[i].dirty;

         //Finally, copy over the data
         copy_page( dest_pt->pt_entries[i].frame_addr * ARCH_FRAME_SIZE,
                    source_pt->pt_entries[i].frame_addr * ARCH_FRAME_SIZE );
      }
   }

   //Now set the page table descriptor 
   dest_pde->present = PAGE_PRESENT;
   dest_pde->rw = PAGE_RW;
   dest_pde->user_access = PAGE_USR_ACCESS;
   dest_pde->table_addr = (VIRT_TO_PHYS( dest_pt ) / ARCH_PAGE_SIZE);
}

pd_t *vm_pdir_clone(pd_t *clone_dir){

   pd_t *new_dir = k_malloc( sizeof(pd_t), ARCH_PAGE_SIZE );
   memset( new_dir, sizeof(pd_t), 0 );

   for(int i = 0; i < PDE_IN_PD; i++){

      //Map in the kernel virtual addresses to the cloned page directory.
      //We unconditionally map the kernel in since, when switching tasks,
      //the scheduler MUST be in the same memory location. Creating a task
      //that does not have this property will cause big troubles.
      if( kernel_page_dir->pd_entries[i].table_addr == 
          clone_dir->pd_entries[i].table_addr ){
         new_dir->pd_entries[i] = kernel_page_dir->pd_entries[i];
      }else{
         //If there is a directory in clone_dir that is not in the 
         //kernel page directory, make a copy of it.
         clone_table(&new_dir->pd_entries[i], &clone_dir->pd_entries[i] );
         new_dir->pd_entries[i].present = 
                  clone_dir->pd_entries[i].present;
         new_dir->pd_entries[i].rw = clone_dir->pd_entries[i].rw;
         new_dir->pd_entries[i].user_access = 
                  clone_dir->pd_entries[i].user_access;
      }
   }
   return new_dir;
}
 
void page_int_handler(context_t r);
void vm_init(){
   //Make the page directory for the kernel
   kernel_page_dir = (pd_t*)k_malloc( sizeof(pd_t), ARCH_PAGE_SIZE );
   memset( kernel_page_dir, sizeof(pd_t), 0 );

   //Create the kernel page table mapping. Map 5M of the kernel starting
   //at KERNEL_VADDR to the first 5M of physical memory
   vm_pmap_range( KERNEL_VADDR, 0, kernel_page_dir, PAGE_RW, 
                 PAGE_KERNEL_ACCESS, 0x500000 );

   //Setup a frame pool
   framepool_create( 0x600000, 100 );

   //Setup the interrupt handler for paging BEFORE enabling paging
   arch_register_interrupt( PAGE_INTERRUPT, page_int_handler);

   //Let the processor know where our page table is
   //and enable paging.
   load_pd( (void*)VIRT_TO_PHYS(kernel_page_dir));
}

//TODO have a page fault callback instead of this.
void page_int_handler(context_t r){
   int fault_addr;
   //TODO convert to assembly
   asm volatile("mov %%cr2, %0" : "=r" (fault_addr));

   uint8_t present = !(r.error & PAGE_PRESENT_M);
   uint8_t rw = r.error & PAGE_RW_M;
   uint8_t us = r.error & PAGE_USR_M;
   uint8_t reserved = r.error & PAGE_RES_M;
   uint8_t id = r.error & PAGE_ID_M;

   k_puts("Page Fault: ");
   if( present )
      k_puts("page not present at ");
   else if(rw)
      k_puts("page read-only at ");
   else if(us)
      k_puts("page ring-0 only at ");
   else if(reserved)
      k_puts("reserved bits overwritten at ");
   else if( id )
      k_puts("instruction fetch at ");
   k_printf("0x%x\n", fault_addr);

   //We currently cannot handle a fault, so
   //halt the machine
   arch_stop_cpu();
}
