#pragma once

#include <lib/types.h>

#define KERNEL_VBASE 0xC0000000

#define ARCH_PAGE_SIZE  0x1000
#define TABLE_SIZE 0x1000
#define PAGE_INTERRUPT 14

//Masks for page error
#define PAGE_PRESENT_M 0x1
#define PAGE_RW_M      0x2
#define PAGE_USR_M     0x4
#define PAGE_RES_M     0x8
#define PAGE_ID_M      0x10

//There are 1024 32-bit entries (pte_t, pde_t, etc) in
//each level of the paging structure
#define ENTRIES_IN_LEVEL	1024

//The upper 10 bits of the address specifies the 
//offset within the page directory.
#define PD_INDEX(vaddr) (vaddr >> 22)

//After the first 10 bits, the next 10 bits specifies
//the offset within the page table.
#define PT_INDEX(vaddr) ((vaddr >> 12) & 0x3ff)

//Used to convert KERNEL Virtual <=> Physical addresses. THIS
//ONLY WORKS FOR KERNEL ADDRESSES! 
#define PHYS_TO_VIRT(paddr) ( (uint32_t)paddr + KERNEL_VBASE)
#define VIRT_TO_PHYS(vaddr) ( (uint32_t)vaddr - KERNEL_VBASE)


//Align a page on 4K boundary
#define ALIGN_4K(address) (address & ~0xFFF)

//Various constants for paging structure
#define PAGE_PRESENT 1
#define PAGE_RW 1
#define PAGE_RO 0
#define PAGE_USR_ACCESS 1
#define PAGE_KERNEL_ACCESS 0

//This represents an individual page<->frame allocation.
struct pte{
   uint8_t present          : 1; //Present in memory
   uint8_t rw               : 1; //Read-write if set
   uint8_t user             : 1; //Ring 0 if clear
   uint8_t write_through    : 1; //
   uint8_t cache_disable    : 1; //Has the page been written to?
   uint8_t accessed         : 1; //Has page been accessed?
   uint8_t dirty            : 1; 
   uint8_t zero_not_used    : 1;
   uint8_t global_page      : 1;
   uint8_t available        : 3; 

   //Since a page MUST be aligned to 4K, only the upper 20 bits
   //matter. This is the frame address >> 12 
   uint32_t frame_addr      : 20; 
} __attribute__((packed));
typedef struct pte pte_t;

//A page table is composed of page table entires.
//Each of these page entries maps a virtual to physical
//address using a page_table_entry_t.
struct pt{
   pte_t pt_entries[ENTRIES_IN_LEVEL];
}__attribute__((packed));
typedef struct pt pt_t;


struct pde{
   int8_t present          : 1;
   int8_t rw               : 1;
   int8_t user_access      : 1; //1 = user access, 0 = ring0 only
   int8_t write_through    : 1;
   int8_t cache_disabled   : 1;
   int8_t accessed         : 1;
   int8_t not_used         : 1;
   int8_t page_size        : 1;
   int8_t global_page      : 1;
   int8_t available        : 3;

   int32_t table_addr      : 20;

} __attribute__((packed));
typedef struct pde pde_t;


//A page directory is composed of page table descriptors.
struct pd{
   pde_t pd_entries[ENTRIES_IN_LEVEL];
}__attribute__((packed));
typedef struct pd pd_t;

///////////////////////////////////////////////

//This points to the current page directory being used
//for address translation. It is null if not page table
//is in effect.
//TODO make {get,set}_kernel_page_dir
extern pd_t *kernel_page_dir;

//Map a physical to virtual address in the paging structure
void vm_pmap(uint32_t vaddr, uint32_t paddr, pd_t *page_directory, 
              uint8_t rw, uint8_t user_access);

//Map a sequential range of pages so we do not have to repeatedly call
//map_page. length is in bytes.
void vm_pmap_range( uint32_t vaddr, uint32_t paddr, pd_t *page_directory,
                     uint8_t rw, uint8_t user_access, uint32_t length);

//Used to temporarily setup a page mapping
void vm_pmap_temp(uint32_t vaddr, uint32_t paddr, pd_t *page_directory);
///////////////////////////////////////

/////////////////Cloning page directory
pd_t *vm_pdir_clone(pd_t *page_directory);


//Given a buffer, length, and physical address, copy the
//buffer to the physical address.
void vm_copy_to_physical(char *vbuf, uint32_t paddr, uint32_t len);

//Copy memory from one page directory into a buffer
void vm_copy_from_pdir(uint32_t vaddr, pd_t *page_directory,
                       char *to, uint32_t len);

void vm_copy_str(uint32_t vaddr, pd_t *page_directory, 
                       char *to, uint32_t max);

//Used to find a mapping between virtual and physical addresses
uint32_t virt_to_phys(uint32_t uaddr, pd_t *page_directory);

//Initilized the paging structure
void vm_init();
