#pragma once

#include <lib/types.h>
#include <lib/memory.h>
#include <lib/bitwise.h>
#include <kernel/mm/heap.h>

//By default, we will allocate blocks of 64 bytes
#define DEFAULT_BLOCKSIZE 64

//Used to "color" the blocks in the dibitmap
#define DIBIT_FREE 0
#define DIBIT_BLUE 0x1
#define DIBIT_RED 0x2
#define DIBIT_GREEN 0x3

typedef struct{
   uint32_t blocksize;

   uint32_t dibitmap_len;
   char *dibitmap;
   
   //Another convenience field.
   //Points to the actual location that
   //data is dynamically allocated to.
   char *free_space;
}bitmap_heap_t;

extern heap_algs_t bitmap_heap;

//Initilizes the heap
void bitmap_init_heap(heap_t *heap);

//Allocates memory on the heap
void *bitmap_malloc(heap_t *heap, uint32_t size, uint32_t align);

//Frees dynamic memory
void bitmap_free(heap_t *heap, void *memblock);
