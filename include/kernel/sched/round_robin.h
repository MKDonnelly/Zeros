#pragma once

#include <kernel/sched/sched.h>
#include <kernel/task.h>
#include <lib/abstract_ll.h>

extern struct sched_alg rr_alg;

void rr_add_task(ktask_t*);
void rr_rm_task(ktask_t*);
void rr_exit_task(void *);
void rr_yield_task();
void *rr_join_task(ktask_t*);
thread_context_t *rr_schedule(thread_context_t*);
void rr_init_scheduler();



