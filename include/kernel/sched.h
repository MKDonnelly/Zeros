#pragma once 

//Generic properties of a scheduler. Describes the
//basic functions a scheduler must implement.

#include <kernel/thread.h>
#include <lib/abstract_ll.h>

struct sched_alg{

   //Removes and adds a thread to
   //be run.
   void (*add_thread)();
   void (*rm_thread)();
   
   //Called when a thread exits
   void (*exit_thread)();
   void (*yield_thread)();
   void (*join_thread)();

   //Main routine run when scheduling
   void (*schedule)();
   void (*init_scheduler)();
};


#define SCHEDULER_INTERRUPT 50

//The arch interrupt handler will put the
//current context in this variable. This can
//be used by the scheduler to swap threads.
extern thread_context_t *cur_context;

struct sched_alg current_sched_alg;

//Initilize the threading system
void init_threading();

