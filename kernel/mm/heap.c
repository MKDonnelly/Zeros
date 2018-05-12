#include <kernel/mm/heap.h>

//TODO clean this up
void create_heap(heap_t *heap_descriptor, uint32_t start, uint32_t len, void *(*malloc_alg)(), void (*free_alg)(), void (*init_heap)() ){

    heap_descriptor->start = start;
    heap_descriptor->len = heap_descriptor->size_left = len;
    heap_descriptor->malloc = malloc_alg;
    heap_descriptor->free = free_alg;
    heap_descriptor->init_heap = init_heap;

    heap_descriptor->init_heap( heap_descriptor );
}
