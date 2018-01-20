#pragma once 

#include <thread.h>
#include <vgacommon.h>
#include <delay.h>
#include <isr.h>
#include <cpu.h>
#include <linked_list.h>
#include <timing.h>
#include <string.h>

typedef llnode_t thread_node_t;

extern thread_context_t *cur_context;

//Limitations:
//  We assume all threads never exit

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
