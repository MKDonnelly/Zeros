#include <arch/x86/lmode/paging.h>
#include <kernel/mm/heap.h>

#define VIRT_TO_PHYS(addr) (addr)
#define PHYS_TO_VIRT(addr) (addr)

pml4t_t *kernel_pdir;

//Retrieves the given pte_t given a page table and virtual address
//If create == 1, entries are created if needed. If create == 0 and
//the higher-levels of the paging structure do not exist, then NULL
//is returned.
static pte_t *get_page( uint64_t vaddr, int create, pml4t_t *page_table){
   //Given a pml4_t, get the page directory pointer, if it exists
   pml4e_t *pml4_entry = &page_table->entries[PML4_OFFSET(vaddr)];

   if( pml4_entry->base_addr == 0 && create ){
      pdpt_t *new_pdpt = k_malloc(sizeof(pdpt_t), 0x1000);
      memset(new_pdpt, sizeof(pdpt_t), 0);
      uint64_t phys = PHYS_TO_VIRT(new_pdpt);

      pml4_entry->base_addr = phys / 0x1000;
      pml4_entry->present = 1;
      pml4_entry->rw = 1; //temporary
      pml4_entry->usr = 1; //temporary
      pml4_entry->page_size = 0; //for 4K paging
      
   }else if( pml4_entry->base_addr == 0 && !create ){
      //We were asked to not create entries, yet we would need to
      return NULL;
   }
   pdpt_t *pdpt_table = (pdpt_t*)PHYS_TO_VIRT(pml4_entry->base_addr * 0x1000);

   pdpe_t *pdpe_entry = &pdpt_table->entries[PDPT_OFFSET(vaddr)];
   if( pdpe_entry->base_addr == 0 && create ){
      pdt_t *new_pdir = k_malloc(sizeof(pdt_t), 0x1000);
      memset(new_pdir, sizeof(pdt_t), 0);
      uint64_t phys = VIRT_TO_PHYS(new_pdir);

      pdpe_entry->base_addr = phys / 0x1000;
      pdpe_entry->present = 1;
      pdpe_entry->rw = 1; //temporary
      pdpe_entry->usr = 1; //temporary
      pdpe_entry->page_size = 0; //for 4K paging
   }else if( pdpe_entry->base_addr == 0 && !create){
      return NULL;
   }
   pdt_t *pdt_table = (pdt_t*)PHYS_TO_VIRT(pdpe_entry->base_addr * 0x1000);

   pde_t *pde_entry = &pdt_table->entries[PDT_OFFSET(vaddr)];
   if( pde_entry->base_addr == 0 && create ){
      pt_t *new_ptable = k_malloc(sizeof(pt_t), 0x1000);
      memset(new_ptable, sizeof(pt_t), 0);
      uint64_t phys = VIRT_TO_PHYS(new_ptable);

      pde_entry->base_addr = phys / 0x1000;
      pde_entry->present = 1;
      pde_entry->rw = 1; //temporary
      pde_entry->usr = 1; //temporary
      pde_entry->page_size = 0; //always 0 for 4K paging
   }else if( pde_entry->base_addr == 0 && !create ){
      return NULL;
   }
   
   pt_t *pt_table = (pt_t*)PHYS_TO_VIRT(pde_entry->base_addr * 0x1000);

   //now get the pde_t from the page directory
   return &pt_table->entries[ PT_OFFSET(vaddr) ];
}

void map_page( uint64_t virtual, uint64_t physical, pml4t_t *pd){
   pte_t *page = get_page(virtual, 1, pd);

   page->base_addr = (physical / 0x1000);
   page->present = 1;
   page->rw = 1;
   page->usr = 1;
}

void init_paging(){
   kernel_pdir = (pml4t_t*)k_malloc(sizeof(pml4t_t), 0x1000);
   memset( kernel_pdir, sizeof(pml4t_t), 0);

   for(uint64_t i = 0; i < 0x300000; i += 0x1000 ){
      map_page( i, i, kernel_pdir ); 
   }

   uint64_t addr = (uint64_t)kernel_pdir;
   //load_pml4( addr );
}
