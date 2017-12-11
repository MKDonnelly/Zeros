#pragma once

#include <types.h>
#include <string.h>
#include <vgacommon.h>
#include <cpu.h>

#define HEAPNODE_SIZE sizeof( struct heapNode )

extern uint32_t kernel_start_heap, kernel_end_heap;

//Experimental heap allocator
struct heapNode{
   //A pointer to the next heap node
   //udword nextChunk; 
   struct heapNode *nextChunk;
   //The size of the chunk of memory,
   //not including this header.
   uint32_t size;
   //Various attributes such as if the
   //memory is free or allocated.
   //ubyte attributes;
   int8_t isAllocated : 1; //Is this being used?
   int8_t not_used : 7; //For future information
}__attribute__((packed));


void init_heap();
void *kmalloc(uint32_t,uint8_t,uint32_t*);
void kfree(void*);
void unify_heap();
