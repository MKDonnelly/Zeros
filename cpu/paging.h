#pragma once

#include "../drivers/vgatext/vgatext.h"
#include "../kernel/kmalloc.h"
#include "../lib/memory.h"

//Page entry bitmap
typedef struct page{
   unsigned int present  : 1; //Present in memory
   unsigned int rw       : 1; //Read-write if set
   unsigned int user     : 1; //Ring 0 if clear
   unsigned int accessed : 1; //Has page been accessed?
   unsigned int dirty    : 1; //Has the page been written to?
   unsigned int unused   : 7; //Unused and reserved bits
   unsigned int frame    : 20; //Frame address w/ 12 bit shift
} page_t;

//A page table is an array of page descriptors
typedef struct page_table{
   page_t pages[1024];
} page_table_t;

//A page directory is an array of page tables
//this is what two-level paging is.
typedef struct page_directory{
   //Array of page tables
   page_table_t *tables[1024];

   //This is the physical location of the 
   //page tables
   unsigned int tablesPhysical[1024];

   unsigned int physicalAddr;
} page_directory_t;

//Initilized the paging structure
void initialise_paging();

//Creates the page directory and
//loads it into CR3.
void switch_page_directory(page_directory_t *new);

//Gets the specified page
page_t *get_page(unsigned int addr, int make, page_directory_t *dir);

