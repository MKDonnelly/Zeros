#pragma once

#include <lib/types.h>
#include <kernel/kmalloc.h>
#include <arch/x86/archx86.h>

enum THREAD_STATE { THREAD_RUNNING, THREAD_EXIT };

//This is a kernel thread. It is designed
//to be used by an abstract linked list
//in the scheduler code.
typedef struct kthread{
   struct kthread *next;

   //Pointer to stack with context
   thread_context_t *context;
   void *stack_ptr; //For freeing dynamic memory
   enum THREAD_STATE state;
   void *return_value;
   int thread_id;
   int priority;

} kthread_t;

//Initilize a thread descriptor. Currently, it passes most of the
//work to create_thread_context
kthread_t *k_create_thread(void *start_function,
   void *params, void *exit_function, uint32_t stack_size);
