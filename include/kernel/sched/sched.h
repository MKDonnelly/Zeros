#pragma once 

//Generic properties of a scheduler. Describes the
//basic functions a scheduler must implement.

#include <kernel/thread.h>
#include <lib/timing.h>
#include <lib/abstract_ll.h>

struct sched_alg{

   //Removes and adds a thread to
   //be run.
   void (*add_thread)();
   void (*rm_thread)();
   
   //Called when a thread exits
   void (*exit_thread)();
   void (*yield_thread)();
   void *(*join_thread)();

   //Main routine run when scheduling
   thread_context_t *(*schedule)();
   void (*init_scheduler)();
};


#define SCHEDULER_INTERRUPT 50

//The arch interrupt handler will put the
//current context in this variable. This can
//be used by the scheduler to swap threads.
extern thread_context_t *cur_context;

//Used by the system to perform scheduling
struct sched_alg *current_sched_alg;
#define k_add_thread( kthread ) current_sched_alg->add_thread( kthread )
#define k_rm_thread( kthread )  current_sched_alg->rm_thread( kthread )
#define k_exit_thread( value )  current_sched_alg->exit_thread( value )
#define k_yield_thread()        current_sched_alg->yield_thread()
#define k_join_thread( kthread ) current_sched_alg->join_thread( kthread )
#define k_init_scheduler()      current_sched_alg->init_scheduler()

//Initilize the threading system
void setup_scheduler();
void start_scheduler();
