#pragma once

#include <arch/x86/pmode/context.h>
#include <arch/x86/pmode/paging.h>

//When we are doing blocking system calls, we end up
//with both the context of the interrupted userland task
//and the context of the kernel land sleeping task on the 
//stack. When returning, we need to pop them back off and
//restore the esp pointers.
typedef struct{
   context_t *stacks[2];
   int current_stack;
}callstack_t;

//Arch-specific structure describing task
//state. This is meant to be part of a larger
//ktask_t structure that is abstracted away from
//the architecture
typedef struct{
   callstack_t callstack;

   //Pointer to the task's kernel-level interrupt stack.
   //Switched in the TSS 
   void *interrupt_stack;

   //Virtual address space for task
   pd_t *task_pd;
}arch_task_t;

arch_task_t arch_ktask_create(void (*start)(), void *params,
                                   void (*exit)(), size_t stack_addr);
arch_task_t arch_utask_create(void (*start)(), void *params,
                                   void (*exit)(), size_t stack_addr);
arch_task_t arch_utask_from_elf(char*,int);
