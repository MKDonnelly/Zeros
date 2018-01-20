#pragma once
#include <types.h>
#include <kmalloc.h>
#include <isr.h>

//The initial EFlags value when starting a thread
#define INITIAL_EFLAGS 0x0202

//A thread context is identical to the register
//list pushed when an interrupt is triggered 
typedef registers_t thread_context_t;

//This is a kernel thread
typedef struct kthread{
   //Pointer to stack with context
   thread_context_t *context;
   void *stack_ptr; //For freeing dynamic memory
   int state;
   int exit_status;
} kthread_t;

//Create the initial state of the thread
void create_thread_context(thread_context_t**,void(func)(void *),void*, void (*thread_exit)(), void*,uint32_t);

//Used by init_threading to jump to the first thread.
//Never returns.
void jump_to_thread(thread_context_t*);

//Initilize a thread descriptor. Currently, it passes most of the
//work to create_thread_context
kthread_t *k_create_thread(void*,void*,void*,uint32_t,int);
