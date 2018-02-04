#include <arch/x86/gdt.h>

const int16_t gdt_kernel_code = 0x8;
const int16_t gdt_kernel_data = 0x10;

//Our global descriptor table
struct gdt_entry GDT[GDT_ENTRIES];
//The GDT descriptor
struct gdt_descriptor GDT_DES;

void init_gdt(){

   //Initilize the null descriptor
   GDT[0].limit = 0;
   GDT[0].baseLower = 0;
   GDT[0].baseMiddle = 0;
   GDT[0].flags = 0;
   GDT[0].flagsAndLimit = 0;
   GDT[0].baseUpper = 0;

   //Initilize the Code segment
   //Have it start at 0x0 and
   //and end at the 4G mark
   GDT[1].limit = 0xffff;
   GDT[1].baseLower = 0x0;
   GDT[1].baseMiddle = 0x0;
   GDT[1].flags = 0b10011010;
   GDT[1].flagsAndLimit = 0b11001111;
   GDT[1].baseUpper = 0x0;

   //Initilize the Data segment
   //Same as w/ the code segment.
   GDT[2].limit = 0xffff;
   GDT[2].baseLower = 0x0;
   GDT[2].baseMiddle = 0x0;
   GDT[2].flags = 0b10010010;
   GDT[2].flagsAndLimit = 0b11001111;
   GDT[2].baseUpper = 0x0;
   
   //Initilize the GDT descriptor
   //Always subtract one.
   GDT_DES.length = sizeof( struct gdt_entry ) * GDT_ENTRIES - 1;
   GDT_DES.address = (uint32_t*)GDT;

   //Load the GDT
   asm volatile("lgdtl (%0)" : : "r" (&GDT_DES));

   //Update segment registers and do a long jump to update cs
   asm volatile("movw %0, %%ax" : : "b"(gdt_kernel_data) :);
   asm volatile("movw %%ax, %%ds" :);
   asm volatile("movw %%ax, %%es" :);
   asm volatile("movw %%ax, %%fs" :);
   asm volatile("movw %%ax, %%gs" :);
   asm volatile("movw %%ax, %%ss" :);
   asm volatile("ljmp %0, $updateCS;  \
                 updateCS: " :: "i"(gdt_kernel_code));
}

