#pragma once 

#include <kernel/thread.h>
#include <lib/abstract_ll.h>

#define SCHEDULER_INTERRUPT 50

//The arch interrupt handler will put the
//current context in this variable. This can
//be used by the scheduler to swap threads.
extern thread_context_t *cur_context;

//Initilize the threading system
void init_threading();

//Called by interrupt (and ONLY interrupt)
//to pick with thread to run next.
void schedule();

//Adds a thread to the list of threads to run.
void add_thread(kthread_t*);

//Removes a thread
void rm_thread(kthread_t*);

//Voluntarily stop a thread
void thread_yield();

//Called when a thread returns
void thread_exit(void*);

//Called to join with the given
//thread with thread_id
//void *thread_join( int thread_id );
void *thread_join( kthread_t *descriptor );
