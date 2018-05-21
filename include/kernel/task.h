#pragma once

#include <lib/types.h>
#include <kernel/mm/heap.h>
#include <arch/x86/archx86.h>
#include <lib/assert.h>

enum TASK_STATE { TASK_RUNNING, TASK_READY, TASK_EXIT, TASK_BLOCKED };

//This is a kernel task. It is designed
//to be used by an abstract linked list
//in the scheduler code.
typedef struct ktask{
   struct ktask *next;

   //Generic, arch-abstracted structure describing
   //the task's state.
   arch_task_t task_info;

   //Generic information related to every task
   void *task_stack; //Pointer to top of stack
   void *task_brk;   //Pointer to end of heap
   uint32_t task_brk_len; //Length of task heap, in bytes
   enum TASK_STATE state;
   void *ret_val;
   uint32_t task_id;
   uint8_t is_kernel_task;

   //Later, I might add a void* to point to a structure to
   //store scheduler-specific data.
} ktask_t;

//When creating a task, we pass in the head of the stack.
//k_malloc will return a pointer to the beginning of the memory
//block, but the head of the stack is at the end of the memory block.
//This converts from a pointer from k_malloc to a stack head pointer
#define STACK_HEAD( ptr, stack_size ) (uint32_t*)( (uint8_t*)ptr + stack_size )
ktask_t *k_create_ktask(void (*start)(), void *param, void (*exit)());

ktask_t *k_create_utask(void (*start)(), void *param,
                        void (*exit)(), uint32_t *stack);

ktask_t *k_create_utask_elf(char *elf_data);
