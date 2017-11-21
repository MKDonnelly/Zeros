#pragma once
#include "../lib/types.h"

// KERNEL_CS describes the index of the segment
// descriptor to reference in the GDT. In this case,
// 0x08 references the first valid segment, or 
// the kernel code segment. This index is also scaled
// by eight by the CPU to get an offset from the GDT.
// This is actually a segment selector.
#define KERNEL_CS 0x08    //My kernel uses this
#define TOTAL_INTERRUPTS 256

//The main structure is the interrupt table (int_table)
//and is composed of an array of 256 idt_entry's.
//each entry within this table is an idt_entry and contains
//information about the specific interrupt.

typedef struct{
   u16 lower_offset;     //Lower 16 bits of memory offset for handler
   u16 segment_sel;      //The kernel segment selector
   u8 zeroed;
   /*Flags byte
    *Bit 7: Is interrupt present?
    *Bits 6-5: Privilege level of caller (0=ring 0, 3=ring 3)
    *Bit 4: Set to 0 for interrupt gate
    *Bits 3-0: 1110 = 14 = 32 bit interrupt gate
    * */   
   u8 flags;
   u16 higher_offset;    //higher 16 bits for offset of handler
}__attribute__((packed)) idt_entry;

typedef struct{
   u16 length;
   u32 base_addr;
}__attribute__((packed)) idt_descriptor;

//The Interrupt descriptor table and 
//interrupt descriptor
idt_entry int_table[TOTAL_INTERRUPTS];
idt_descriptor idt_des;

//Given an interrupt number and a 
//function handler, place it into
//the IDT.
void add_idt_entry(u8, u32);
//This does the job of actually
//loading the IDT to the system.
void load_idt();
