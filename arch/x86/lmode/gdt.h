#pragma once

#include <lib/bitwise.h>
#include <lib/types.h>

//Note: most of the gdt fields are ignored.
//The only flags that are recognized are
//   Conforming
//   Descriptor Privilege level
//   Present
//   L bit
//   D bit
//When initilized, I always set conforming to 0, present to 1, 
//the L bit to 1, and the D bit to 0. So the only field that is
//worth setting is the DPL
typedef struct{
   uint32_t first_half; //All ignored
   uint32_t second_half; //most ignored
}gdt_entry_t;

typedef struct{
   uint16_t limit;
   int64_t base;   
}gdt_descriptor_t;


static inline void setup_gdt_entry(gdt_entry_t *gdt_entry, uint8_t dpl){
   gdt_entry->second_half = gdt_entry->second_half | (dpl << 13);

   //Conforming bit
   bit_clear(&(gdt_entry->second_half), 10);

   //No read by processor, but manual says to set to 1
   bit_set(&(gdt_entry->second_half), 11);
   bit_set(&(gdt_entry->second_half), 12);

   //Present
   bit_set(&(gdt_entry->second_half), 15);

   //Long mode code segment bit
   bit_set(&(gdt_entry->second_half), 21);

   //D bit
   bit_clear(&(gdt_entry->second_half), 22);
}

