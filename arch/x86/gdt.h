#pragma once

#include <lib/types.h>

//Null, kernel data/code, userland
//data/code, TSS
#define GDT_ENTRIES 6

//Our specific GDT (initilized in init_gdt()
//will be layed out with the code segment first
//and the data segment second.
extern const int16_t gdt_kernel_code;
extern const int16_t gdt_kernel_data;

//Used to update the Segment Registers
//when initilizing the GDT.
extern void srupdate();

//TODO improve the descriptions, make a function to do each
//gdt entry, have flags named by bit
struct gdt_entry{
   int16_t limit;        //Limit bits 015
   int16_t baseLower;    //Base bits 0-15
   int8_t  baseMiddle;    //Base bits 16-23

/*      flags bits description
  bit 0: present flag. Is this segment present in memory?
         1 for yes, 0 for not
  bit 1-2: Ring level for this descriptor. 00 for ring 0,
           and 11 for ing 3
  bit 3: descriptor type. system segment (0) or a code/data
         segment (1)
  bit 4: is this a code segment (1) or a data segment (0)
  bit 5: conforming bit. 1 if code from a lower ring level
         can call code in this segment, 0 else
  bit 6: is this segment rw (1) or r-only (0)
  bit 7: has this segment been accessed? (1 if yes, 0 else)
*/

   int8_t  flags;         //1st flags and type flags

/*     2nd group of flags description
  bit 0: granularity. are limit/base int terms of 4K (1) or 1b (0)
  bit 1: operand size. 0 for 16 bit, 1 for 32 bit
  bit 2: 1 for 64 bit mode code segment, 0 for no
  bit 3: 1 for available for system use, 0 else
*/


   int8_t  flagsAndLimit; //2nd flags, limit bits 16-19
   int8_t  baseUpper;     //Base bits 24-31
} __attribute__((packed));


struct gdt_descriptor{
   uint16_t length;
   uint32_t *address;
} __attribute__((packed));

extern struct gdt_entry GDT[GDT_ENTRIES];
extern struct gdt_descriptor GDT_DES;

//We will just automatically set
//the GDT entries.
//There is no need to pass them as args
//since we only need a few entries.
//We can modify this later as needed.
void init_gdt();
