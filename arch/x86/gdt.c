#include <arch/x86/gdt.h>

/////////////////TEMPORARY
#include <lib/memory.h>
#include <arch/x86/tss.h>

void make_tss(int32_t num, uint16_t ss0, uint32_t esp0){

   //compute base and limit of entry
   uint32_t base = (uint32_t)&tss_entry;
   uint32_t limit = base + sizeof(tss_entry);

   //Add tss to gdt
   GDT[5] = (struct gdt_entry){
      .limit_low = (limit & 0xFFFF),
      .base_low = (base & 0xFFFF),
      .base_mid = (base >> 16) & 0xFF,

      .accessed = 1,
      .rw = 0,
      .conforming = 0,
      .code_or_data = 1,
      .descriptor_type = 0,
      .ring_level = 3,
      .present = 1,

      .available = 0,
      .long_mode_code = 0,
      .operand_size = 1,
      .granularity = 1,
      .lim_high = 0xf,
      .base_high = (base >> 24) & 0xFF
   };

   //Zero out TSS
   memset( &tss_entry, sizeof(tss_entry), 0 );

   //Ser kernel stack segment and pointer
   tss_entry.ss_ring0 = ss0;
   tss_entry.esp_ring0 = esp0;

   tss_entry.cs = 0x0b;
   tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
}

void set_kernel_stack(uint32_t stack){
   tss_entry.esp_ring0 = stack;
}

/////////////////////////

const int16_t gdt_kernel_code = 0x8;
const int16_t gdt_kernel_data = 0x10;
const int16_t gdt_userland_code = 0x18;
//Used when userland is calling a GDT descriptor
//The last three bits are set to indicate RPL = 0b11
const int16_t gdt_userland_code_rpl = 0x1B;
const int16_t gdt_userland_data = 0x20;
const int16_t gdt_userland_data_rpl = 0x23;

//Our global descriptor table
struct gdt_entry GDT[GDT_ENTRIES];

//The GDT descriptor
struct gdt_descriptor GDT_DES;

void init_gdt(){

   //Initilize the null descriptor
   memset( (void*)&GDT[0], sizeof(struct gdt_entry), 0 );

   //Initilize the kernel code segment
   //Have it start at 0x0 and
   //and end at the 4G mark

   GDT[1] = (struct gdt_entry){
      .limit_low = 0,
      .base_mid = 0,

      //Flags
      .present = 1,
      .ring_level = 0,
      .descriptor_type = 1,
      .code_or_data = 1,
      .conforming = 0,
      .rw = 1,
      .accessed = 0,

      .available = 0,
      .long_mode_code = 0,
      .operand_size = 1,
      .granularity = 1,
      .lim_high = 0xf,
      .base_high = 0
   };

   //Initilize the kernel data segment
   //Same as w/ the code segment. 
   GDT[2] = (struct gdt_entry){
      .limit_low = 0xffff,
      .base_low = 0,
      .base_mid = 0,

      .present = 1,
      .ring_level = 0,
      .descriptor_type = 1,
      .code_or_data = 0,
      .conforming = 0,
      .rw = 1,
      .accessed = 0,

      .available = 0,
      .long_mode_code = 0,
      .operand_size = 1,
      .granularity = 1,
      .lim_high = 0xf,
      .base_high = 0
   };

   GDT[3] = (struct gdt_entry){
      .limit_low = 0xffff,
      .base_low = 0,
      .base_mid = 0,

      .accessed = 0,
      .rw = 1,
      .conforming = 1,
      .code_or_data = 1,
      .descriptor_type = 1,
      .ring_level = 3,
      .present = 1,
  
      .available = 0,
      .long_mode_code = 0,
      .operand_size = 1,
      .granularity = 1,
      .lim_high = 0xf,
      .base_high = 0,
   };


   GDT[4] = (struct gdt_entry){
      .limit_low = 0xffff,
      .base_low = 0,
      .base_mid = 0,

      .accessed = 0,
      .rw = 1,
      .conforming = 1,
      .code_or_data = 0,
      .descriptor_type = 1,
      .ring_level = 3,
      .present = 1,

      .available = 0,
      .long_mode_code = 0,
      .granularity = 1,
      .lim_high = 0xf,
      .base_high = 0
   };

   //Let kmain set the kernel stack
   make_tss( 5, gdt_kernel_data, 0x0 );

   //Initilize the GDT descriptor
   //Always subtract one.
   GDT_DES.length = sizeof( struct gdt_entry ) * GDT_ENTRIES - 1;
   GDT_DES.address = (uint32_t*)GDT;

   //Load the GDT
   extern void load_gdt(uint32_t);
   load_gdt((uint32_t)&GDT_DES);

   //Load the TSS now that the GDT is in place
   load_tss();
}
