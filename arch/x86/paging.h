#pragma once

/*                        Paging
The paging structure is setup so that the page directory
contains 1024 page tables, which in turn contains 1024 page entries.
Page tables and page entries can be null to indicate that it is not
used. 
*/


#include <arch/x86/drivers/vgacommon/vgacommon.h>
#include <arch/x86/isr.h>
#include <arch/x86/frame.h>

#include <kernel/mm/heap.h>

#include <lib/memory.h>
#include <lib/bitwise.h>
#include <lib/types.h>

#define ARCH_PAGE_SIZE  0x1000
#define TABLE_SIZE 0x1000

#define KERNEL_MEMORY 0
#define USER_MEMORY   1
#define IS_WRITEABLE  1
#define NOT_WRITEABLE 0

#define PAGE_INTERRUPT 14

//Masks for page error
#define PAGE_PRESENT_M 0x1
#define PAGE_RW_M      0x2
#define PAGE_USR_M     0x4
#define PAGE_RES_M     0x8
#define PAGE_ID_M      0x10

//PD = page directory
//PT = page table
//PE = page entry
#define PT_PER_PD 1024
#define PE_PER_PT 1024


//This describes an individual page
//which maps virtual addresses to physical
//addresses.
typedef struct page{
   uint32_t present  : 1; //Present in memory
   uint32_t rw       : 1; //Read-write if set
   uint32_t user     : 1; //Ring 0 if clear
   uint32_t accessed : 1; //Has page been accessed?
   uint32_t dirty    : 1; //Has the page been written to?
   uint32_t unused   : 7; //Unused and reserved bits
   uint32_t frame    : 20; //Frame address w/ 12 bit shift
   //!!IMPORTANT!! That 12 bit shift means that the frame
   //              is now an INDEX
   //This is the physical address of memory mapped to this page.
   //Only 20 bits are used as every page must begin on 4K boundaries.
} page_entry_t;



//A page table is an array of page entries 
typedef struct page_table{
   page_entry_t pages[1024];
} page_table_t;


//A page directory is an array of page tables
//this is what two-level paging is.
typedef struct page_directory{
   //Array of page tables
   page_table_t *tables[1024];

   //Physical address of the page tables
   //above for use when loading into CR3
   uint32_t tablesPhysical[1024];

   //The physical address of tablesPhysical
   uint32_t physicalAddr;
} page_directory_t;


//This will be visible to the kernel for use
extern page_directory_t *kernel_page_dir;
extern page_directory_t *kernel_ref_dir;


//Map a virtual to physical address using paging.
//Contrast this to <next function> which maps the
//given virtual address to the next free physcial
//address. This allows greater control.
uint8_t page_map(page_entry_t*,uint8_t,uint8_t,uint32_t);

//Get a page from the page table
page_entry_t *get_page(uint32_t, uint8_t, page_directory_t*);

uint32_t get_frame(page_entry_t*);

//Initilized the paging structure
void init_paging();

//Creates the page directory and
//loads it into CR3.
void load_page_dir(page_directory_t*);

//Handles page interrupts
void page_int_handler(registers_t);

extern void copy_page_physical(uint32_t,uint32_t);
page_directory_t *clone_dir(page_directory_t*);
