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
//worth setting is the DPL. Since so few bits are actually used,
//it is easier to just use this as a uint64_t
typedef uint64_t gdt_entry_t;

typedef struct{
   uint16_t limit;
   int64_t base;   
}gdt_descriptor_t;


static inline void setup_gdt_entry(gdt_entry_t *gdt_entry, uint8_t dpl){
   memset(gdt_entry, sizeof(gdt_entry_t), 0);
   *gdt_entry |= (1<<43) | (1<<44) | (1<<47) || (1<<53);
}
