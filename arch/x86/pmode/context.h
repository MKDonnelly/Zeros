#pragma once

#include <lib/types.h>
#include <arch/x86/pmode/isr.h>

//The initial EFlags value when starting a thread
#define INITIAL_EFLAGS 0x0202

//A context is the registers pushed when interrupted 
typedef registers_t context_t;

//Create an intial context for a new kernel task
context_t *arch_create_kcontext( void (*start)(void*), void *param,
                                 void (*exit)(), uint32_t *stack_addr );
context_t *arch_create_ucontext( void (*start)(void*), void *param,
                                 void (*exit)(), uint32_t *stack_addr );
