#include <arch/x86/lmode/paging.h>
#include <kernel/mm/heap.h>
#include <arch/x86/lmode/pagingasm.h>
#include <lib/assert.h>

static inline uint64_t virt_to_phys(uint64_t addr){
   return addr;
}

static inline uint64_t phys_to_virt(uint64_t addr){
   return addr;
}

pml4t_t *kernel_pdir;

//Retrieves the given pte_t given a page table and virtual address
//If create == 1, entries are created if needed. If create == 0 and
//the higher-levels of the paging structure do not exist, then NULL
//is returned.
static pte_t *get_page( uint64_t vaddr, int create, pml4t_t *page_table){
   KASSERT(page_table != NULL);

   //Given a pml4_t, get the page directory pointer, if it exists
   pml4e_t *pml4_entry = &page_table->entries[PML4_OFFSET(vaddr)];
   KASSERT(pml4_entry != NULL);

   if( pml4_entry->base_addr == 0 && create ){
      pdpt_t *new_pdpt = k_malloc(sizeof(pdpt_t), 0x1000);
      KASSERT(new_pdpt != NULL);
      memset(new_pdpt, sizeof(pdpt_t), 0);

      pml4_entry->base_addr = phys_to_virt((uint64_t)new_pdpt) / 0x1000;
      pml4_entry->present = 1;
      pml4_entry->rw = 1; //temporary
      pml4_entry->usr = 1; //temporary
      pml4_entry->page_size = 0; //for 4K paging
      
   }else if( pml4_entry->base_addr == 0 && !create ){
      //We were asked to not create entries, yet we would need to
      return NULL;
   }
   pdpt_t *pdpt_table = (pdpt_t*)phys_to_virt(
                              (uint64_t)(pml4_entry->base_addr * 0x1000));
   KASSERT(pdpt_table != NULL);

   pdpe_t *pdpe_entry = &pdpt_table->entries[PDPT_OFFSET(vaddr)];
   KASSERT(pdpe_entry != NULL);
   if( pdpe_entry->base_addr == 0 && create ){
      pdt_t *new_pdir = k_malloc(sizeof(pdt_t), 0x1000);
      KASSERT(new_pdir != NULL);
      memset(new_pdir, sizeof(pdt_t), 0);

      pdpe_entry->base_addr = virt_to_phys((uint64_t)new_pdir) / 0x1000;
      pdpe_entry->present = 1;
      pdpe_entry->rw = 1; //temporary
      pdpe_entry->usr = 1; //temporary
      pdpe_entry->page_size = 0; //for 4K paging
   }else if( pdpe_entry->base_addr == 0 && !create){
      return NULL;
   }
   pdt_t *pdt_table = (pdt_t*)phys_to_virt(
                               (uint64_t)(pdpe_entry->base_addr * 0x1000));
   KASSERT(pdt_table != NULL);

   pde_t *pde_entry = &pdt_table->entries[PDT_OFFSET(vaddr)];
   KASSERT(pde_entry != NULL);
   if( pde_entry->base_addr == 0 && create ){
      pt_t *new_ptable = k_malloc(sizeof(pt_t), 0x1000);
      KASSERT(new_ptable != NULL);
      memset(new_ptable, sizeof(pt_t), 0);

      pde_entry->base_addr = virt_to_phys((uint64_t)new_ptable) / 0x1000;
      pde_entry->present = 1;
      pde_entry->rw = 1; //temporary
      pde_entry->usr = 1; //temporary
      pde_entry->page_size = 0; //always 0 for 4K paging
   }else if( pde_entry->base_addr == 0 && !create ){
      return NULL;
   }
   
   pt_t *pt_table = (pt_t*)phys_to_virt(
                                (uint64_t)(pde_entry->base_addr * 0x1000));
   KASSERT(pt_table != NULL);

   //now get the pde_t from the page directory
   return &pt_table->entries[ PT_OFFSET(vaddr) ];
}

void vm_pmap( uint64_t virtual, uint64_t physical, pml4t_t *pd, 
              uint8_t flags){
   pte_t *page = get_page(virtual, 1, pd);
   KASSERT(page != NULL);

   page->base_addr = (physical / 0x1000);
   page->present = 1;
   if( flags & VMAP_W)
      page->rw = 1;
   else
      page->rw = 0;

   if( flags & VMAP_USR )
      page->usr = 1;
   else
      page->usr = 0;
}

void vm_init(){
   kernel_pdir = (pml4t_t*)k_malloc(sizeof(pml4t_t), 0x1000);
   KASSERT( kernel_pdir != NULL );
   memset( kernel_pdir, sizeof(pml4t_t), 0);

   for(uint64_t i = 0; i < 0x300000; i += 0x1000 ){
      vm_pmap( i, i, kernel_pdir, VMAP_W | VMAP_USR );
   }

   //Test to make sure the mapping worked
   for(uint64_t i = 0; i < 0x300000; i+= 0x1000){
      pte_t *page = get_page(i, 0, kernel_pdir);
      KASSERT( i == (page->base_addr * 0x1000) );
   }

   load_pml4( (uint64_t)kernel_pdir );
}
