#pragma once
#include <lib/types.h>

//NOTE: Only 4K pages are supported
//maybe I will add 4M and 1G pages later

//The following is needed to select 4K pages
//   CR4.PAE enabled (already done, since we are in LM)
//   PDPE.PS = 0 (we set this here)
//   PDE.PS = 0 (we set this here)

//Each level contains 512 8-byte entries
#define ENTRIES_IN_LEVEL	512

//Macros to find the offset of an address
//within a specific page entity
#define PML4_OFFSET(addr) ( (addr >> 39) & 0x1FF )
#define PDPT_OFFSET(addr) ( (addr >> 30) & 0x1FF )
#define PDT_OFFSET(addr) ( (addr >> 21) & 0x1FF )
#define PT_OFFSET(addr) ( (addr >> 12) & 0x1FF )

//Flags meaning
//   present - page is present if set to 1. PF if accessed and 0
//   rw - if 1, r/w is allowed, else ro
//   usr - if usr == 1, ring-3 can access, else only ring0 can access
//   pwt -
//   pcd -
//   accessed - Set by processor if page accessed
//   no_exec - when set and EFER MSR (NXE, bit 11) set, 
//             no execution allowed from page

//64 bits
typedef struct pte{
   //Contains info about a page
   uint64_t present : 1;
   uint64_t rw      : 1;
   uint64_t usr     : 1;
   uint64_t pwt     : 1;
   uint64_t pcd     : 1;
   uint64_t accessed : 1;
   uint64_t d_bit   : 1; //dirty bit
   uint64_t pat     : 1;
   uint64_t g_bit   : 1; //global page bit
   uint64_t avl     : 3;
   uint64_t base_addr : 40;
   uint64_t os_available : 11;
   uint64_t no_exec : 1;
}pte_t;

typedef struct pt{
   //contains page table entries
   pte_t entries[ENTRIES_IN_LEVEL];
}pt_t;

//64 bits
typedef struct pde{
   //Points to a page table
   uint64_t present : 1;
   uint64_t rw      : 1;
   uint64_t usr     : 1;
   uint64_t pwt     : 1;
   uint64_t pcd     : 1;
   uint64_t accessed : 1;
   uint64_t ignored : 1;
   uint64_t page_size : 1; //always 0 for 4K
   uint64_t ignored2 : 1;
   uint64_t avl     : 3;
   uint64_t base_addr : 40;
   uint64_t os_available : 11;
   uint64_t no_exec : 1;
}pde_t;

typedef struct pdt{
   //Contains pde_t's
   pde_t entries[ENTRIES_IN_LEVEL];
}pdt_t;

//64 bits
typedef struct pdpe{
   //Points to a page directory table
   uint64_t present : 1;
   uint64_t rw      : 1;
   uint64_t usr     : 1;
   uint64_t pwt     : 1;
   uint64_t pcd     : 1;
   uint64_t accessed : 1;
   uint64_t ignored : 1;
   uint64_t page_size : 1; //Always 0 for 4K paging
   uint64_t ignored2 : 1;
   uint64_t avl     : 3;
   uint64_t base_addr : 40;
   uint64_t os_available : 11;
   uint64_t no_exec : 1;
}pdpe_t;

typedef struct pdpt{
   //Contains pdpe_t's
   pdpe_t entries[ENTRIES_IN_LEVEL];
}pdpt_t;

//64 bits in length
typedef struct pml4e{
   //Points to a Page Directory Pointer Table
   uint64_t present : 1;
   uint64_t rw      : 1;
   uint64_t usr     : 1;
   uint64_t pwt     : 1;
   uint64_t pcd     : 1;
   uint64_t accessed : 1;
   uint64_t ignored : 1;
   uint64_t page_size : 1; //Always 0 for 4K paging
   uint64_t ignored2 : 1;
   uint64_t avl     : 3;
   uint64_t base_addr : 40;
   uint64_t os_available : 11;
   uint64_t no_exec : 1;
}__attribute__((packed)) pml4e_t;

typedef struct pml4t{
   //contains PML4E's
   pml4e_t entries[ENTRIES_IN_LEVEL];
}pml4t_t;

void vm_init();
