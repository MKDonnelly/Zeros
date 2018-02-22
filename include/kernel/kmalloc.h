#pragma once

#include <lib/types.h>
#include <arch/x86/archx86.h>

//When splitting a piece of free memory in the
//heap, this is the minimum amount of free memory
//that we need left over to make a split. Anything less
//and kmalloc will just merge it into the called memory.
#define MIN_SPLIT 20

extern uint32_t kernel_start_heap, kernel_end_heap;

typedef struct heap{
   uint32_t start;
   uint32_t len;
   uint32_t size_left;
}heap_t;

typedef struct heapnode{
   //Pointer to the next heapNode
   //in the heap 
   struct heapnode *next_node;
   
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
} heapnode_t;

//Initilize the kernel heap
void init_heap();

//Dynamically allocate memory
void *kmalloc(uint32_t size, uint32_t align, uint32_t *phys);

//Free dynamic memory
void kfree(void*);
