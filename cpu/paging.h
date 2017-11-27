#pragma once

#include <vgatext/vgatext.h>
#include <isr.h>
#include <kmalloc.h>
#include <memory.h>
#include <bitwise.h>
#include <types.h>
#include <string.h>

#define PAGE_SIZE  0x1000
#define FRAME_SIZE 0x1000
#define TABLE_SIZE 0x1000

#define KERNEL_MEMORY 0
#define USER_MEMORY   1
#define IS_WRITEABLE  1
#define NOT_WRITEABLE 0

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
   //!!IMPORTANT!! That 12 bit shift means that the frame
   //              is now an INDEX
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
extern page_directory *kernel_page_dir;

//A bitset to determine which frames
//have been allocated.
extern char* frames;

//Map a virtual to physical address using paging.
//Contrast this to <next function> which maps the
//given virtual address to the next free physcial
//address. This allows greater control.
int page_map(page_entry*,ubyte,ubyte,int);

//Allocate a frame automatically to the page_entry
int page_map_auto(page_entry*,ubyte,ubyte);

//Un-associates a frame from a page_entry
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
