//#include <arch/x86/archx64.h>
#include <arch/current_arch>

#include <lib/print.h>
#include <lib/string.h>
#include <kernel/multiboot.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/bitmap_heap.h>
#include <kernel/mm/blocklist_heap.h>

extern size_t ldscript_kernel_end;

void kmain64(struct multiboot_header *mbh){
   arch_init_system();

   heap_create( &global_kernel_heap,(size_t)&ldscript_kernel_end, 0x200000,
                &blocklist_heap);
   char *mem = k_malloc( 1024, 0 );
   k_free(mem);
   k_puts("Working..."); 

   while(1) arch_halt_cpu();
}
