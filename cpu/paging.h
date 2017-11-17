#pragma once

#include "../drivers/vgatext/vgatext.h"
#include "../cpu/isr.h"
#include "../kernel/kmalloc.h"
#include "../lib/memory.h"
#include "../lib/bitwise.h"
#include "../lib/types.h"
#include "../lib/string.h"

#define PAGE_SIZE  0x1000
#define FRAME_SIZE 0x1000
#define PAGE_INTERRUPT 14

//This describes an individual page
//which maps virtual addresses to physical
//addresses.
typedef struct page{
   unsigned int present  : 1; //Present in memory
   unsigned int rw       : 1; //Read-write if set
   unsigned int user     : 1; //Ring 0 if clear
   unsigned int accessed : 1; //Has page been accessed?
   unsigned int dirty    : 1; //Has the page been written to?
   unsigned int unused   : 7; //Unused and reserved bits
   unsigned int frame    : 20; //Frame address w/ 12 bit shift
   //This is the physical address of memory mapped to this page.
   //Only 20 bits are used as every page must begin on 4K boundaries.
} page_entry;

//A page table is an array of page entries 
typedef struct page_table{
   page_entry pages[1024];
} page_table;

//A page directory is an array of page tables
//this is what two-level paging is.
typedef struct page_directory{
   //Array of page tables
   page_table *tables[1024];

   //Physical address of the page tables
   //above.
   unsigned int tablesPhysical[1024];

   //The physical address of tablesPhysical
   unsigned int physicalAddr;
} page_directory;

//This will be visible to the kernel so
//that is may allocate pages
extern page_directory *sys_page_table;
extern unsigned int *frames;

//Allocate a frame
int alloc_frame(page_entry*,int,int);
void free_frame(page_entry*);
//Get a page from the page table
page_entry *get_page(unsigned int, int, page_directory*);

//Initilized the paging structure
void init_paging();

//Creates the page directory and
//loads it into CR3.
void load_page_dir(page_directory*);

//Gets the specified page
page_entry *get_page(unsigned int, int, page_directory*);

//Handles page interrupts
void page_int_handler(struct registers);
