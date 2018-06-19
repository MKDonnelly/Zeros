#include <arch/current_arch>

#include <lib/print.h>
#include <lib/string.h>
#include <kernel/multiboot.h>
#include <kernel/mm/heap.h>
#include <kernel/mm/bitmap_heap.h>
#include <kernel/mm/blocklist_heap.h>
#include <arch/x86/drivers/lapic.h>

extern size_t ldscript_kernel_end;

void kmain64(struct multiboot_header *mbh){
   arch_init_system();

   heap_create( &global_kernel_heap,(size_t)&ldscript_kernel_end, 0x100000,
                &blocklist_heap);
   vm_init();
   apic_enable();
   k_puts("Working..."); 

   while(1) arch_halt_cpu();
}
