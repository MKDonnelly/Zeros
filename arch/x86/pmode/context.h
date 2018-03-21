#pragma once

#include <lib/types.h>
#include <arch/x86/pmode/isr.h>

//The initial EFlags value when starting a thread
#define INITIAL_EFLAGS 0x0202

//A thread context is identical to the register
//list pushed when an interrupt is triggered 
typedef registers_t thread_context_t;

//Create the initial state of the thread
void arch_create_kernel_context(thread_context_t**,void(func)(void *),void*, void (*thread_exit)(), void*,uint32_t);
void arch_create_userland_context(thread_context_t**,void(func)(void *),void*, void (*thread_exit)(), void*,uint32_t);

