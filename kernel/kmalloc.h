#pragma once

#include <types.h>
#include <string.h>
#include <vgacommon.h>
#include <cpu.h>

#define HEAPNODE_SIZE sizeof(struct heapNode)

extern uint32_t kernel_start_heap, kernel_end_heap;

struct heapNode{
   //Pointer to the next heapNode
   //in the heap 
   struct heapNode *nextNode;
   
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
}__attribute__((packed));

//Initilize the kernel heap
void init_heap();

//Dynamically allocate memory
void *kmalloc(uint32_t,uint8_t,uint32_t*);

//Free dynamic memory
void kfree(void*);
