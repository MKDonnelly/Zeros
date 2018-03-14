#pragma once

#include <arch/x86/drivers/vgacommon/vgacommon.h>
#include <arch/x86/isr.h>
#include <arch/x86/frame.h>

#include <kernel/mm/heap.h>

#include <lib/memory.h>
#include <lib/bitwise.h>
#include <lib/types.h>

#define ARCH_PAGE_SIZE  0x1000
#define TABLE_SIZE 0x1000

#define PAGE_INTERRUPT 14

//Masks for page error
#define PAGE_PRESENT_M 0x1
#define PAGE_RW_M      0x2
#define PAGE_USR_M     0x4
#define PAGE_RES_M     0x8
#define PAGE_ID_M      0x10

//PDES = Page descriptor
//PT = Page Table
//PTDES = Page table descriptors
//PD = Page directory
#define PDES_IN_PT 1024
#define PTDES_IN_PD 1024

//The upper 10 bits of the address specifies the 
//offset within the page directory.
#define offset_in_pd(addr) (addr >> 22)

//After the first 10 bits, the next 10 bits specifies
//the offset within the page table.
#define offset_in_pt(addr) ((addr >> 12) & 0x3ff)


//This represents an individual page<->frame allocation.
struct page_descriptor{
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
typedef struct page_descriptor page_desc_t;

//A page table is composed of page table entires.
//Each of these page entries maps a virtual to physical
//address using a page_table_entry_t.
struct page_table{

   page_desc_t page_descriptors[ PDES_IN_PT ];

}__attribute__((packed));
typedef struct page_table page_table_t;


//Each of these points to a single page_table_t,
//The address of which is in table_addr.
struct page_table_desc{
   int8_t present          : 1;
   int8_t rw               : 1;
   int8_t user_access      : 1;
   int8_t write_through    : 1;
   int8_t cache_disabled   : 1;
   int8_t accessed         : 1;
   int8_t not_used         : 1;
   int8_t page_size        : 1;
   int8_t global_page      : 1;
   int8_t available        : 3;

   int32_t table_addr      : 20;

} __attribute__((packed));
typedef struct page_table_desc page_table_desc_t;


//A page directory is composed of page table descriptors.
struct page_directory{

   page_table_desc_t pt_descriptors[PTDES_IN_PD];

}__attribute__((packed));
typedef struct page_directory page_directory_t;


//This points to the current page directory being used
//for address translation. It is null if not page table
//is in effect.
extern page_directory_t *current_directory;

/////////////////Get information from paging structures

//Given a virtual address and page directory, get the page table that
//the address resides in.
page_table_t *get_page_table(uint32_t addr, page_directory_t *dir);

//Given a virtual address and a page table, retrieve the page
//descriptor that maps the virtual address to a physical address.
page_desc_t *get_page_desc(uint32_t addr, page_table_t *table);

//Given a page table descriptor, get the frame address from it.
uint32_t get_frame(page_desc_t *page);
//////////////////


/////////////////Cloning page directory
page_directory_t *clone_page_dir(page_directory_t*);

//extern void copy_page_physical(uint32_t,uint32_t);
page_directory_t *clone_dir(page_directory_t*);
/////////////////


//Given a virtual address and a page directory, find the physical
//address that the virtual address maps to
uint32_t virt_to_phys(uint32_t addr, page_directory_t *dir);

//Loads the given page directory into CR3 for use. 
void load_page_dir(page_directory_t*);

//Given a buffer, length, and physical address, copy the
//buffer to the physical address.
void copy_to_physical(char*,int,uint32_t);

void map_page(uint32_t vaddr, uint32_t paddr, page_directory_t *dir);

//This will be visible to the kernel for use
//Userland processes will start with a clone of
//this page table as well.
extern page_directory_t *kernel_page_dir;

//Initilized the paging structure
void init_paging();

//Handles page interrupts
void page_int_handler(registers_t);

//Defined in page.asm
void arch_disable_paging();
void arch_enable_paging();
void load_page_dir(page_directory_t*);
