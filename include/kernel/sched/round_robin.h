#pragma once
#include <kernel/task.h>
#include <lib/abstract_ll.h>

void rr_add_task(ktask_t*);
void rr_rm_task(ktask_t*);
void rr_exit_task(void*);
void rr_yield_task();
void *rr_join_task(ktask_t*);
void rr_schedule();
void rr_setup_scheduler();
void rr_start_scheduler();
