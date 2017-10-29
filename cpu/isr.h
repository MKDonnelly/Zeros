
#pragma once

#include "idt.h"
#include "../drivers/vgatext/vgatext.h"
#include "../drivers/portio.h"
#include "../drivers/timing.h"

#include "../lib/string.h"
#include "../lib/types.h"

extern unsigned char keyboard_map[128];

//Defined in assembly.asm
extern void isr0();

extern void isr30();
extern void isr31();
extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();

//When calling an interrupt, 
//the registers are pushed
//to the stack. This is the
//structure of those registers
//which a c function can use.
//Pusha pushes in the following order:
//eax, ecx, edx, ebx, esp (original value),
//ebp, esi, edi.
//In addition, an interrupt pushes an error code
//and interrupt number. Read this backwards and from
//bottom to top to see how the registers are pushed.
//eflags, then returnCS, then returnEIP, then error, then
//int_number ...
struct registers{
   udword dataSegment;
   udword edi, esi, ebp, esp, ebx, edx, ecx, eax;
   ubyte int_number, error;
   //automatically pushed by cpu
   udword returnEIP, returnCS, eflags;
}__attribute__((packed));

//This is where each handler 


//This function handles the installation
//of each interrupt handler
void install_interrupts();

//All interrupts must pass through this to
//be routed to the correct destination.
void main_interrupt_handler(struct registers);
