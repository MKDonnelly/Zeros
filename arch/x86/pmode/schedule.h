#pragma once
#include <lib/types.h>
#include <lib/timing.h>
#include <arch/x86/pmode/arch_task.h>

#define ARCH_SCHED_INT 50

//Handles arch-level tasks for scheduling.
//This includes mapping the interrupt. In addition,
//higher-level kernel code will pass in arch_task_info_t*,
//which will be used by this to run the task.
void setup_sched(void (*sched_callback)(), uint32_t ms_period);
void handle_sched_int();
void arch_run_next(arch_task_info_t *next_task);
