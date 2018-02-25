#pragma once

#include <kernel/sched.h>
#include <kernel/thread.h>
#include <lib/abstract_ll.h>

extern struct sched_alg rr_alg;

void rr_add_thread(kthread_t*);
void rr_rm_thread(kthread_t*);
void rr_exit_thread();
void rr_yield_thread();
void rr_join_thread();
thread_context_t *rr_schedule(thread_context_t*);
void rr_init_scheduler();



