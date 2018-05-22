#pragma once

#include <arch/x86/pmode/context.h>
#include <arch/x86/pmode/paging.h>

//Arch-specific structure describing task
//state. This is meant to be part of a larger
//ktask_t structure that is abstracted away from
//the architecture
typedef struct{
   //Pointer to the thread stack (contains context_t)
   context_t *task_context;   

   //Pointer to the task's kernel-level interrupt stack.
   //Switched in the TSS 
   void *interrupt_stack;

   //Virtual address space for task
   pd_t *task_pd;
}arch_task_t;

arch_task_t arch_ktask_create(void (*start)(), void *params,
                                   void (*exit)(), uint32_t *stack);
arch_task_t arch_utask_create(void (*start)(), void *params,
                                   void (*exit)(), uint32_t *stack);
arch_task_t arch_utask_from_elf(char*);
