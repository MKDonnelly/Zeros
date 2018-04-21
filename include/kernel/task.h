#pragma once

#include <lib/types.h>
#include <kernel/mm/heap.h>
#include <kernel/sched/sched.h>
#include <arch/x86/archx86.h>

#ifdef NO
enum TASK_STATE { TASK_RUNNING, TASK_READY, TASK_EXIT, TASK_BLOCKED };

//This is a kernel task. It is designed
//to be used by an abstract linked list
//in the scheduler code.
typedef struct ktask{
   struct ktask *next;

   //Generic, arch-abstracted structure describing
   //the task's state.
   arch_task_info_t task_info;

   //Generic information related to every task
   enum TASK_STATE state;
   void *ret_val;
   uint32_t task_id;
   uint8_t is_kernel_task;

   //Later, I might add a void* to point to a structure to
   //store scheduler-specific data.
} ktask_t;

//Initilize a task descriptor. Currently, it passes most of the
//work to create_thread_context
ktask_t *k_create_kernel_task(void *start_function,
   void *params, void *exit_function, uint32_t stack_size,pd_t*);

ktask_t *k_create_userland_task(void *start_function,
   void *params, void *exit_function, uint32_t stack_size, 
   uint32_t stack_addr, pd_t*);

//Have the task voluntarily yield the cpu. This can also
//be used for blocking a task when certain system calls 
//are used.
void task_yield();

//Called to exit the current thread
void task_exit(void*);

//Have one thread join up with another
void *task_join( ktask_t*);
#endif
