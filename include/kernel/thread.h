#pragma once
#include <lib/types.h>
#include <kernel/kmalloc.h>
#include <arch/x86/isr.h>
#include <arch/x86/context.h>

enum THREAD_STATE { THREAD_RUNNING, THREAD_EXIT };

//This is a kernel thread
typedef struct kthread{
   //Pointer to stack with context
   thread_context_t *context;
   void *stack_ptr; //For freeing dynamic memory
   enum THREAD_STATE state;
   int exit_status;
} kthread_t;

//Initilize a thread descriptor. Currently, it passes most of the
//work to create_thread_context
kthread_t *k_create_thread(void *start_function,
   void *params, void *exit_function, uint32_t stack_size);
