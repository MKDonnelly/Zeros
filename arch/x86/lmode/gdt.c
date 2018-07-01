#include <arch/x86/lmode/gdt.h>

static gdt_entry_t __attribute__((aligned(16))) gdt[2];
static gdt_descriptor_t gdt_desc;

//Found in descriptors.asm
extern void gdt_load(gdt_descriptor_t *new_gdt);

static inline void add_entry(gdt_entry_t *gdt_entry, uint8_t dpl){
   memset(gdt_entry, sizeof(gdt_entry_t), 0);
   *gdt_entry |= (1L<<43) | (1L<<44) | (1L<<47) | (1L<<53);
}

void gdt_init(){
   gdt[0] = 0;

   //Kernel code
   add_entry(&gdt[1], 0);

   gdt_desc.base = (int64_t)gdt;
   gdt_desc.limit = sizeof(gdt_entry_t) * 2;

   gdt_load(&gdt_desc);
}
