#pragma once

#include <lib/bitwise.h>
#include <lib/types.h>
#include <lib/string.h>

//Note: most of the gdt fields are ignored.
//The only flags that are recognized are
//   Conforming
//   Descriptor Privilege level
//   Present
//   L bit
//   D bit
//When initilized, I always set conforming to 0, present to 1, 
//the L bit to 1, and the D bit to 0. So the only field that is
//worth setting is the DPL. Since so few bits are actually used,
//it is easier to just use this as a uint64_t
typedef uint64_t gdt_entry_t;

typedef struct{
   uint16_t limit;
   int64_t base;   
}gdt_descriptor_t;

//Initilize the GDT for tye system
void gdt_init();
