
#pragma once
#include "types.h"

//Look in the GDT. 0x08 is eight 
//bytes from the start of the gdt,
//which is exactly where the gdt code 
//section is.
#define KERNEL_CS 0x08

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

#define IDT_ENTRIES 256
idt_entry int_table[IDT_ENTRIES];
idt_descriptor idt_des;

//Functions in idt.c
void add_idt_entry(int, u32);
void load_idt();



