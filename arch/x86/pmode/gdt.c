#include <arch/x86/pmode/gdt.h>

#include <lib/memory.h>
#include <arch/x86/pmode/tss.h>
#include <arch/x86/pmode/descriptors.h>

//Our global descriptor table
static gdt_entry_t gdt_table[GDT_ENTRIES];

//The GDT descriptor
static gdt_descriptor_t gdt_descriptor;

//First GDT entry (after NULL)
const int16_t gdt_kernel_code = 0x8;

//Second GDT entry
const int16_t gdt_kernel_data = 0x10;

//Third GDT entry
const int16_t gdt_userland_code = 0x18;

//Fourth GDT entry
const int16_t gdt_userland_data = 0x20;

//Used when userland is calling a GDT descriptor
//The last three bits are set to indicate RPL = 0b11
const int16_t gdt_userland_code_rpl = 0x1B;
const int16_t gdt_userland_data_rpl = 0x23;

void gdt_set_entry( gdt_entry_t *gdt_entry, uint32_t limit, 
                    uint32_t base, uint32_t flags ){

   gdt_entry->limit_low = (limit & 0xFFFF);
   gdt_entry->lim_high = (limit >> 16) & 0xF;

   gdt_entry->base_low = (base & 0xFFFF);
   gdt_entry->base_mid = (base >> 16) & 0xFF;
   gdt_entry->base_high = (base >> 24) & 0xFF; 

   gdt_entry->accessed = (flags & GDT_ACCESSED) ? 1 : 0;
   gdt_entry->rw       = (flags & GDT_RW )      ? 1 : 0;
   gdt_entry->conforming = (flags & GDT_CONFORM ) ? 1 : 0;
   gdt_entry->code_or_data = (flags & GDT_CODET) ? 1 : 0;
   gdt_entry->descriptor_type = (flags & GDT_CDTYPE) ? 1 : 0;
   gdt_entry->ring_level = (flags & GDT_RLEVEL_3);

   //Always set to present
   gdt_entry->present = 1;

   //Always 0 since this is protected mode code
   gdt_entry->long_mode_code = 0; 

   //Always 1 since we want a 32 bit operand size
   gdt_entry->operand_size = 1;

   gdt_entry->granularity = (flags & GDT_GRAN4K) ? 1 : 0;
}


void gdt_init(){

   //Initilize the null descriptor
   memset( gdt_table, sizeof(gdt_entry_t), 0 );

   //Initilize the kernel code segment
   //Have it start at 0x0 and
   //and end at the 4G mark
   gdt_set_entry( &gdt_table[1],  0xFFFFFF, 0x0, GDT_RW | GDT_CODET | 
                  GDT_CDTYPE | GDT_RLEVEL_0 | GDT_GRAN4K );


   //Initilize the kernel data segment
   //Same as w/ the code segment. 
   gdt_set_entry( &gdt_table[2], 0xFFFFFF, 0x0, GDT_RW | GDT_DATAT | 
                   GDT_CDTYPE | GDT_RLEVEL_0 | GDT_GRAN4K);

   //Set userland code segment
   gdt_set_entry( &gdt_table[3], 0xFFFFFF, 0x0, GDT_RW | GDT_CODET |
                   GDT_CDTYPE | GDT_RLEVEL_3 | GDT_GRAN4K );


   //Make userland data segment
   gdt_set_entry( &gdt_table[4], 0xFFFFFF, 0x0, GDT_RW | GDT_DATAT |
                   GDT_CDTYPE | GDT_RLEVEL_3 | GDT_GRAN4K );

   //Create the TSS
   tss_create( &gdt_table[5], gdt_kernel_data );

   //Initilize the GDT descriptor
   //Always subtract one for length
   gdt_descriptor.length = sizeof(gdt_entry_t) * GDT_ENTRIES - 1;
   gdt_descriptor.address = (uint32_t*)gdt_table;

   //Load the GDT
   gdt_load(&gdt_descriptor);

   //Load the TSS now that the GDT is in place
   tss_load();
}
