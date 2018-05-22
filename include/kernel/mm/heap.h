#pragma once

#include <lib/types.h>
#include <arch/current_arch>

//Individual malloc algs will create this.
typedef struct{
   void *(*malloc)();
   void (*free)();
   void (*init_heap)();
}heap_algs_t;

typedef struct heap{
   //Start address of heap
   uint32_t start;

   //Lenght of heap, in bytes
   uint32_t len;

   //Approximate number of bytes of
   //free space remaining.
   uint32_t size_left;

   //Specific heap allocation algorithms will
   //put their functions inside of this.
   heap_algs_t actions;

   //This is to prevent the heap from being corrupted when
   //being modified.
   mutex_t heap_lock;
}heap_t;


//Used throughout the kernel code and initilized in kmain
heap_t global_kernel_heap;

#define k_malloc( size, alignment ) \
    global_kernel_heap.actions.malloc(&global_kernel_heap, size, alignment)
#define k_free( memblock ) \
    global_kernel_heap.actions.free(&global_kernel_heap, memblock)

void heap_create(heap_t *heap, uint32_t start_addr, uint32_t len,
                 heap_algs_t *algs );

