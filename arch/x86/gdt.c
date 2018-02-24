#include <arch/x86/gdt.h>

/////////////////TEMPORARY
#include <lib/memory.h>
extern void tss_flush(void);
struct tss{
   uint32_t previous_tss;
   uint32_t esp_ring0, ss_ring0;
   uint32_t esp1_unused;
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
   uint32_t es, cs, ss, ds, fs, gs;
   uint32_t ldt;
   uint16_t trap;
   uint16_t iomap_base;
}__attribute__((packed));
typedef struct tss tss_t;


tss_t tss_entry;

// Set the value of one GDT entry.
void gdt_set_gate(int32_t num, int32_t base, int32_t limit, int32_t access, int8_t gran)
{
   GDT[num].baseLower    = (base & 0xFFFF);
   GDT[num].baseMiddle = (base >> 16) & 0xFF;
   GDT[num].baseUpper   = (base >> 24) & 0xFF;

   GDT[num].limit   = (limit & 0xFFFF);
   GDT[num].flagsAndLimit = (limit >> 16) & 0x0F;

   GDT[num].flagsAndLimit |= gran & 0xF0;
   GDT[num].flags      = access;
} 

void write_tss(int32_t num, uint16_t ss0, uint32_t esp0){

   //compute base and limit of entry
   uint32_t base = (uint32_t)&tss_entry;
   uint32_t limit = base + sizeof(tss_entry);

   //Add tss to gdt
   GDT[5].limit = (limit & 0xFFFF);
   GDT[5].baseLower = (base & 0xFFFF);
   GDT[5].baseMiddle = (base >> 16) & 0xFF;
   GDT[5].flags = 0xE9;
   GDT[5].flagsAndLimit = 0b11000000;
   GDT[5].baseUpper = (base >> 24) & 0xFF;
   //gdt_set_gate( 5, base, limit, 0xE9, 0x00 );

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

   //Initilize the kernel code segment
   //Have it start at 0x0 and
   //and end at the 4G mark
   GDT[1].limit = 0xffff;
   GDT[1].baseLower = 0x0;
   GDT[1].baseMiddle = 0x0;
   GDT[1].flags = 0b10011010;
   GDT[1].flagsAndLimit = 0b11001111;
   GDT[1].baseUpper = 0x0;

   //Initilize the kernel data segment
   //Same as w/ the code segment.
   GDT[2].limit = 0xffff;
   GDT[2].baseLower = 0x0;
   GDT[2].baseMiddle = 0x0;
   GDT[2].flags = 0b10010010;
   GDT[2].flagsAndLimit = 0b11001111;
   GDT[2].baseUpper = 0x0;
   
   //Initilize the userland code segment
   GDT[3].limit = 0xffff;
   GDT[3].baseLower = 0x0;
   GDT[3].baseMiddle = 0x0;
   GDT[3].flags = 0b11111110;
   GDT[3].flagsAndLimit = 0b11001111;
   GDT[3].baseUpper = 0x0;

   //Initilize the userland data segment
   GDT[4].limit = 0xffff;
   GDT[4].baseLower = 0x0;
   GDT[4].baseMiddle = 0x0;
   GDT[4].flags = 0b11110110;
   GDT[4].flagsAndLimit = 0b11001111;
   GDT[4].baseUpper = 0x0;

   write_tss( 5, 0x10, 0x300000 );

   //Initilize the GDT descriptor
   //Always subtract one.
   GDT_DES.length = sizeof( struct gdt_entry ) * GDT_ENTRIES - 1;
   GDT_DES.address = (uint32_t*)GDT;

   //Load the GDT
   asm volatile("lgdtl (%0)" : : "r" (&GDT_DES));

   tss_flush();

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
