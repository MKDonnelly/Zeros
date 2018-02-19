#pragma once

#include <lib/types.h>
#include <arch/x86/gdt.h>
#include <arch/x86/cpu.h>

#define TOTAL_INTERRUPTS 256

//The main structure is the interrupt table (int_table)
//and is composed of an array of 256 idt_entry's.
//each entry within this table is an idt_entry and contains
//information about the specific interrupt.

typedef struct{
   uint16_t lower_offset;     //Lower 16 bits of memory offset for handler
   uint16_t segment_sel;      //The kernel segment selector
   uint8_t  zeroed;
   /*Flags byte
    *Bit 7: Is interrupt present?
    *Bits 6-5: Privilege level of caller (0=ring 0, 3=ring 3)
    *Bit 4: Set to 0 for interrupt gate
    *Bits 3-0: 1110 = 14 = 32 bit interrupt gate
    * */   
   uint8_t flags;
   uint16_t higher_offset;    //higher 16 bits for offset of handler
}__attribute__((packed)) idt_entry_t;

typedef struct{
   uint16_t length;
   uint32_t base_addr;
}__attribute__((packed)) idt_descriptor_t;

//The Interrupt descriptor table and 
//interrupt descriptor
idt_entry_t IDT_TABLE[ TOTAL_INTERRUPTS ];
idt_descriptor_t IDT_DESCRIPTOR;

//Given an interrupt number and a 
//function handler, place it into
//the IDT.
void add_idt_entry(uint8_t, uint32_t);

//This does the job of actually
//loading the IDT to the system.
void load_idt();
