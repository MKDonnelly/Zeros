#pragma once

#include <arch/x86/pmode/arch_task.h>

#define ARCH_SCHED_INT 50

//Kernel-level code passes in the arch_task_t part of a ktask_t
//to instruct the arch layer to run that task next.
void arch_run_next(arch_task_t *next_task);

//Used to explicitly call the scheduler (like with yield)
void arch_trigger_scheduler();

//This sets up the function to call when arch_trigger_scheduler is called
void arch_register_scheduler( void (*sched_handler)());
