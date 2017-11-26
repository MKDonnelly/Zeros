#pragma once

#include "../lib/types.h"
#include "../lib/string.h"
#include "../drivers/vgatext/vgatext.h"

extern unsigned int kernel_start_heap, kernel_end_heap;
void *kmalloc(int,int,unsigned int*);

//Experimental heap allocator
struct heapNode{
   //A pointer to the next heap node
   //udword nextChunk; 
   struct heapNode *nextChunk;
   //The size of the chunk of memory,
   //not including this header.
   udword size;
   //Various attributes such as if the
   //memory is free or allocated.
   //ubyte attributes;
   ubyte isAllocated : 1; //Is this being used?
   ubyte not_used : 7;
};

void heap_init();
void *exp_kmalloc(int,int,unsigned int*);
void exp_kfree(void*);
void unify_heap();
