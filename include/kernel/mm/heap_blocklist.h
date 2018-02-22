#pragma once

#include <lib/types.h>
#include <arch/x86/archx86.h>
#include <kernel/mm/heap.h>

//When splitting a piece of free memory in the
//heap, this is the minimum amount of free memory
//that we need left over to make a split. Anything less
//and kmalloc will just merge it into the called memory.
#define MIN_SPLIT 20


typedef struct heap_block{
   //Pointer to the next heapNode
   //in the heap 
   struct heap_block *next_block;
   
   //The start address of the free memory
   //in this chunk. We need this since the first
   //byte of memory may not be at the end of this 
   //header (we may align furthur in the chunk). 
   void *free_mem;

   //The size of this chunk of memory,
   //not including the header
   uint32_t size;

   //Various attributes such as if the
   //memory is free or allocated.
   int8_t allocated : 1; //Is this being used?
   int8_t not_used : 7; //For future information
} heap_block_t;

//Initilize the kernel heap
void blocklist_init_heap(heap_t*);

//Dynamically allocate memory
void *blocklist_malloc(heap_t*,uint32_t size, uint32_t align, uint32_t *phys);

//Free dynamic memory
void blocklist_free(heap_t*,void*);
