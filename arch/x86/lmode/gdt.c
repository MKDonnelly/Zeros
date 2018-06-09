#include <arch/x86/lmode/gdt.h>

static gdt_entry_t gdt[2];
static gdt_descriptor_t gdt_desc;

void init_gdt(){
   gdt[0] = 0;
   //Kernel code
   setup_gdt_entry(&gdt[1], 0);

   gdt_desc.base = (int64_t*)gdt;
   gdt_desc.limit = sizeof(gdt_entry_t) * 2;

   extern void gdt_load(gdt_descriptor_t*);
   gdt_load(&gdt_desc);
}
