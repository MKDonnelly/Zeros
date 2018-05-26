#include <arch/x86/pmode/tss.h>

#include <arch/x86/pmode/gdt.h>
#include <lib/string.h>

//The TSS used by the system
static tss_t tss_entry;

void tss_create(gdt_entry_t *tss_gdt_entry, uint16_t stack_gdt_segment){

   gdt_set_entry( tss_gdt_entry, sizeof(tss_t), (uint32_t)&tss_entry,
                          GDT_ACCESSED | GDT_RO | GDT_CODET | 
                          GDT_STYPE | GDT_RLEVEL_3 | GDT_GRAN4K );
   //Zero out TSS
   memset( &tss_entry, sizeof(tss_entry), 0 );

   //Set kernel stack segment and pointer
   tss_entry.ss_ring0 = stack_gdt_segment;

   //Kernel MUST call set_kernel_stack to set this 
   //before a system call occurrs
   tss_entry.esp_ring0 = 0x0;

   //The tss is a userland segment (or else if couldn't be called).
   //To let it actually execute kernel land code, we must set its
   //descriptors to reference kernel land memory. In addition, we must
   //set the RPL bits (bits 0 and 1) to the ring level we are calling from.
   tss_entry.cs = gdt_kernel_code | 0x3;
   tss_entry.ss = tss_entry.ds = tss_entry.es = 
         tss_entry.fs = tss_entry.gs = gdt_kernel_data | 0x3;
}

void tss_set_kstack( uint32_t stack_addr ){
   tss_entry.esp_ring0 = stack_addr;
}
