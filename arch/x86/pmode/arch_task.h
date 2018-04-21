#pragma once

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
}arch_task_info_t;
