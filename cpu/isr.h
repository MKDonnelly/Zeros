#pragma once

#include <idt.h>
#include <vgacommon/vgacommon.h>
#include <pic.h>

#include <types.h>
#include <bitwise.h>

extern void init_idt();

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
typedef struct{
   uint32_t dataSegment;
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
   uint8_t int_number, error;
   //automatically pushed by cpu
   uint32_t returnEIP, returnCS, eflags;
}__attribute__((packed)) registers_t;

//This is where each handler is held when registered
void (*int_handlers[TOTAL_INTERRUPTS])(registers_t);

//This is a boolean array to signify that the interrupt
//handler of the given number is present. We will use
//32 bytes since there are 256 interrupts (32 * 8)
uint8_t int_present[ TOTAL_INTERRUPTS / 8 ];

//This function handles the installation
//of each interrupt handler
void init_interrupts();

//This function registers the interrupt so that
//main_interrupt_handler may use it
void register_interrupt( uint8_t number, void (*handler)(registers_t));
void unregister_interrupt( uint8_t );

//All interrupts must pass through this to
//be routed to the correct destination.
void main_interrupt_handler(registers_t);

