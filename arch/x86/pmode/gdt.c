#include <arch/x86/pmode/gdt.h>

#include <lib/memory.h>
#include <arch/x86/pmode/tss.h>
#include <arch/x86/pmode/descriptors.h>

//Our global descriptor table
static gdt_entry_t gdt_table[GDT_ENTRIES];

//The GDT descriptor
static gdt_descriptor_t gdt_descriptor;

//The TSS used by the system
static tss_t tss_entry;

const int16_t gdt_kernel_code = 0x8;
const int16_t gdt_kernel_data = 0x10;
const int16_t gdt_userland_code = 0x18;
const int16_t gdt_userland_data = 0x20;

//Used when userland is calling a GDT descriptor
//The last three bits are set to indicate RPL = 0b11
const int16_t gdt_userland_code_rpl = 0x1B;
const int16_t gdt_userland_data_rpl = 0x23;

#define GDT_CODE 1
#define GDT_DATA 0

//Only calculates and sets the limit and base. Since there are
//so many flags, we need to set those individually else we end
//up with a function of 8+ arguments.
void set_gdt( gdt_entry_t *entry, uint32_t limit, uint32_t base ){

   entry->limit_low = (limit & 0xFFFF);
   entry->lim_high = (limit >> 16) & 0xF;

   entry->base_low = (base & 0xFFFF);
   entry->base_mid = (base >> 16) & 0xFF;
   entry->base_high = (base >> 24) & 0xFF; 
}


void tss_create(int32_t num, uint16_t stack_gdt_segment){

   //compute base and limit of entry
   uint32_t base = (uint32_t)&tss_entry;
   uint32_t limit = base + sizeof(tss_entry);

   //Add tss to gdt
   //make_gdt_entry( num, limit, base, 0, GDT_CODE, 3, 0 );
   set_gdt( &gdt_table[num], limit, base );
   gdt_table[num].accessed = 1;
   gdt_table[num].code_or_data = GDT_CODE;
   gdt_table[num].ring_level = 3;
   gdt_table[num].present = 1;
   gdt_table[num].operand_size = 1;
   gdt_table[num].granularity = 1;   

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
   tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = gdt_kernel_data | 0x3;
}

//used by the kernel to set the interrupt stack
//for system calls.
void set_kernel_stack(uint32_t stack){
   tss_entry.esp_ring0 = stack;
}


void gdt_init(){

   //Initilize the null descriptor
   //TODO just pass in gdt_table
   memset( (void*)&gdt_table[0], sizeof(gdt_entry_t), 0 );

   //Initilize the kernel code segment
   //Have it start at 0x0 and
   //and end at the 4G mark
   set_gdt( &gdt_table[1], 0xFFFFFF, 0x0);
   gdt_table[1].rw = 1;
   gdt_table[1].code_or_data = GDT_CODE;
   gdt_table[1].descriptor_type = 1;
   gdt_table[1].ring_level = 0;
   gdt_table[1].present = 1;
   gdt_table[1].operand_size = 1;
   gdt_table[1].granularity = 1;   


   //Initilize the kernel data segment
   //Same as w/ the code segment. 
   set_gdt( &gdt_table[2], 0xFFFFFF, 0x0);
   gdt_table[2].rw = 1;
   gdt_table[2].code_or_data = GDT_DATA;
   gdt_table[2].descriptor_type = 1;
   gdt_table[2].ring_level = 0;
   gdt_table[2].present = 1;
   gdt_table[2].operand_size = 1;
   gdt_table[2].granularity = 1;   

   //Set userland code segment
   set_gdt( &gdt_table[3], 0xFFFFFF, 0x0);
   gdt_table[3].rw = 1;
   gdt_table[3].code_or_data = GDT_CODE;
   gdt_table[3].descriptor_type = 1;
   gdt_table[3].ring_level = 3;
   gdt_table[3].present = 1;
   gdt_table[3].operand_size = 1;
   gdt_table[3].granularity = 1;   


   //Make userland data segment
   set_gdt( &gdt_table[4], 0xFFFFFF, 0x0);
   gdt_table[4].rw = 1;
   gdt_table[4].code_or_data = GDT_DATA;
   gdt_table[4].descriptor_type = 1;
   gdt_table[4].ring_level = 3;
   gdt_table[4].present = 1;
   gdt_table[4].operand_size = 1;
   gdt_table[4].granularity = 1;   


   //Let kmain set the kernel stack
   tss_create( 5, gdt_kernel_data );

   //Initilize the GDT descriptor
   //Always subtract one for length
   gdt_descriptor.length = sizeof(gdt_entry_t) * GDT_ENTRIES - 1;
   gdt_descriptor.address = (uint32_t*)gdt_table;

   //Load the GDT
   gdt_load(&gdt_descriptor);

   //Load the TSS now that the GDT is in place
   tss_load();
}
