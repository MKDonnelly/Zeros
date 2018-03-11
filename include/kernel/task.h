#pragma once

#include <lib/types.h>
#include <kernel/mm/heap.h>
#include <kernel/sched/sched.h>
#include <arch/x86/archx86.h>

enum TASK_STATE { TASK_RUNNING, TASK_READY, TASK_EXIT, TASK_BLOCKED };

//This is a kernel task. It is designed
//to be used by an abstract linked list
//in the scheduler code.
typedef struct ktask{
   struct ktask *next;

   //Pointer to stack with context
   thread_context_t *context;
   void *stack_ptr; //For freeing dynamic memory

   page_directory_t *task_page_directory;

   enum TASK_STATE state;
   void *return_value;
   int task_id;
   int priority;

} ktask_t;

//Initilize a task descriptor. Currently, it passes most of the
//work to create_thread_context
ktask_t *k_create_kernel_task(void *start_function,
   void *params, void *exit_function, uint32_t stack_size,page_directory_t*);

ktask_t *k_create_userland_task(void *start_function,
   void *params, void *exit_function, uint32_t stack_size, 
   uint32_t stack_addr, page_directory_t*);

//Have the task voluntarily yield the cpu. This can also
//be used for blocking a task when certain system calls 
//are used.
void task_yield();

//Called to exit the current thread
void task_exit(void*);

//Have one thread join up with another
void *task_join( ktask_t*);
