#pragma once

#include <lib/types.h>

typedef struct heap{
   uint32_t start;
   uint32_t len;
   uint32_t size_left;

   //The functions to allocate on the heap will be
   //within the heap descriptor. This is to allow
   //many heap allocation algorithms to operate on
   //any heap. Specific algorithms decide what parameters
   //to accept.
   void *(*malloc)();
   void (*free)();
   void (*init_heap)();
   
}heap_t;

//Used by kernel and initilized in kmain
heap_t kernel_heap;

#define k_malloc(heap_descriptor, size, alignment) heap_descriptor.malloc( &heap_descriptor, size, alignment )

#define k_free(heap_descriptor, memblock) heap_descriptor.free( &heap_descriptor, memblock)

void create_heap(heap_t*,uint32_t,uint32_t,void *(*malloc)(), void (*free)(), void (*init)() );

