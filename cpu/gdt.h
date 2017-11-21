#pragma once

#include "../lib/types.h"

#define GDT_ENTRIES 3
extern void srupdate();

// 1st flags: (segment is present) 1 (ring level)00 
//            (descriptor type)1 -> 0b1001
// type flags: (code segment?)1 (allow low-prive access?)0
//             (readable)1 (accessed)0 -> 0b1010
// 2nd flags: (granularity)1 (32-bit)1 (64-bit)0 (AVL)0 -> 0b1100 
struct gdt_entry{
   uword limit;        //Limit bits 015
   uword baseLower;    //Base bits 0-15
   ubyte baseMiddle;    //Base bits 16-23
   ubyte flags;         //1st flags and type flags
   ubyte flagsAndLimit; //2nd flags, limit bits 16-19
   ubyte baseUpper;     //Base bits 24-31
} __attribute__((packed));

struct gdt_descriptor{
   uword length;
   udword *address;
} __attribute__((packed));

extern struct gdt_entry GDT[GDT_ENTRIES];
extern struct gdt_descriptor GDT_DES;

//We will just automatically set
//the GDT entries.
//There is no need to pass them as args
//since we only need a few entries.
void init_gdt();
