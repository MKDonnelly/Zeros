#include <kernel/mm/heap.h>

void create_heap(heap_t *heap_descriptor, uint32_t start, uint32_t len, 
                 heap_algs_t *algs){

    heap_descriptor->start = start;
    heap_descriptor->len = heap_descriptor->size_left = len;
    heap_descriptor->actions.malloc = algs->malloc;
    heap_descriptor->actions.free = algs->free;
    heap_descriptor->actions.init_heap = algs->init_heap;

    heap_descriptor->actions.init_heap( heap_descriptor );
}
