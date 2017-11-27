#pragma once

#include <types.h>

#define GDT_ENTRIES 3
//Our specific GDT (initilized in init_gdt()
//will be layed out with the code segment first
//and the data segment second.
#define GDT_CODE_SEG 0x8
#define GDT_DATA_SEG 0x10

//Used to update the Segment Registers
//when initilizing the GDT.
extern void srupdate();

// 1st flags: (segment is present) 1 (ring level)00 
//            (descriptor type)1 -> 0b1001
// type flags: (code segment?)1 (allow low-prive access?)0
//             (readable)1 (accessed)0 -> 0b1010
// 2nd flags: (granularity 4K (1) or 1 byte (0))1 (32-bit)1 
//            (64-bit)0 (AVL)0 -> 0b1100 
struct gdt_entry{
   int16_t limit;        //Limit bits 015
   int16_t baseLower;    //Base bits 0-15
   int8_t  baseMiddle;    //Base bits 16-23
   int8_t  flags;         //1st flags and type flags
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
