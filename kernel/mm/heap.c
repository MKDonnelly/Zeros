#include <kernel/mm/heap.h>

void heap_create(heap_t *heap_descriptor, size_t start, size_t len, 
                 heap_algs_t *algs){

    heap_descriptor->start = start;
    heap_descriptor->len = heap_descriptor->size_left = len;
    heap_descriptor->actions.malloc = algs->malloc;
    heap_descriptor->actions.free = algs->free;
    heap_descriptor->actions.init_heap = algs->init_heap;
    spinlock_init( &heap_descriptor->heap_lock );

    heap_descriptor->actions.init_heap( heap_descriptor );
}
