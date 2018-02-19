#pragma once

#include <lib/types.h>
#include <arch/x86/archx86.h>

//When splitting a piece of free memory in the
//heap, this is the minimum amount of free memory
//that we need left over to make a split. Anything less
//and kmalloc will just merge it into the called memory.
#define MIN_SPLIT 20

extern uint32_t kernel_start_heap, kernel_end_heap;

typedef struct heapnode{
   //Pointer to the next heapNode
   //in the heap 
   struct heapnode *nextNode;
   
   //The start address of the free memory
   //in this chunk. It will be at the 
   //end of this header.
   void *freeMem;

   //The size of this chunk of memory,
   //not including the header
   uint32_t size;

   //Various attributes such as if the
   //memory is free or allocated.
   int8_t isAllocated : 1; //Is this being used?
   int8_t not_used : 7; //For future information
} heapnode_t;

//Initilize the kernel heap
void init_heap();

//Dynamically allocate memory
#define KMALLOC_ALIGN 1
#define KMALLOC_NO_ALIGN 0
void *kmalloc(uint32_t size, uint8_t align, uint32_t *phys);

//Free dynamic memory
void kfree(void*);
