#pragma once

#include <arch/x86/pmode/context.h>
#include <lib/types.h>

#define ARCH_MAX_INTERRUPTS 256

//In int.asm
extern void init_idt();

//This function handles the installation
//of each interrupt handler
void init_interrupts();

//This function registers the interrupt so that
//main_interrupt_handler may use it
void arch_register_interrupt(uint8_t number, void (*handler)(context_t));
void arch_unregister_interrupt(uint8_t);
