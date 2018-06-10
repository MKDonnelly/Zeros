#pragma once

#include <lib/types.h>

//When an interrupt is called, the context of the interrupted
//task is saved. The registers are pushed from the bottom of this
//structure to the top
typedef struct{
   uint32_t gs, fs, es, ds;

   //In interrupt_common, we do a pusha. This pushes the esp
   //value WHICH WAS USED BY THE ISR. This is not the task esp
   //when we were interrupted; that value is in esp_pushed.
   uint32_t edi, esi, ebp, USELESS_ESP_VALUE, ebx, edx, ecx, eax;

   //int_number and error are 8 bit numbers, but the
   //stack is aligned to 32 bits.
   uint32_t int_number, error;

   //automatically pushed by cpu
   uint32_t eip, cs, eflags, esp_pushed, ss;
} context_t;


//The initial EFlags value when starting a thread
#define INITIAL_EFLAGS 0x0202

//Create an intial context for a new kernel task
context_t *arch_kcontext_create( void (*start)(void*), void *param,
                                 void (*exit)(), size_t stack_addr );
context_t *arch_ucontext_create( void (*start)(void*), void *param,
                                 void (*exit)(), size_t stack_addr );
