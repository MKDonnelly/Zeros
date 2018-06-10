//#include <arch/x86/archx64.h>
#include <arch/current_arch>

#include <lib/print.h>
#include <lib/string.h>
#include <kernel/multiboot.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/bitmap_heap.h>

extern uint64_t ldscript_kernel_end;

void kmain64(struct multiboot_header *mbh){
   arch_init_system();

   k_puts("Working..."); 

   while(1) arch_halt_cpu();
}
