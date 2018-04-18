#pragma once 

//Generic properties of a scheduler. Describes the
//basic functions a scheduler must implement.

#include <kernel/task.h>
#include <lib/timing.h>
#include <lib/abstract_ll.h>

#define SCHEDULER_INTERRUPT 50

struct sched_alg{

   //Removes and adds a task to be run
   void (*add_task)();
   void (*rm_task)();
   
   //Called when a task exits
   void (*exit_task)();
   void (*yield_task)();
   void *(*join_task)();

   //Main routine run when scheduling
   thread_context_t *(*schedule)();

   //init_scheduler sets up the scheduler 
   //so that tasks may be added before the 
   //actual scheduling happens.
   void (*init_scheduler)();

   //When this is called, the scheduler is
   //started and tasks begin executing
   void (*start_scheduler)();
};

//The arch interrupt handler will put the
//current context in this variable. This can
//be used by the scheduler to swap tasks.
extern thread_context_t *cur_context;

//Used by the system to perform scheduling
struct sched_alg *current_sched_alg;
#define k_add_task( ktask )   current_sched_alg->add_task( ktask )
#define k_rm_task( ktask )    current_sched_alg->rm_task( ktask )
#define k_exit_task( retvalue )    current_sched_alg->exit_task( retvalue )
#define k_yield_task()          current_sched_alg->yield_task()
#define k_join_task( ktask )  current_sched_alg->join_task( ktask )
#define k_init_scheduler()      current_sched_alg->init_scheduler()
#define k_start_scheduler()     current_sched_alg->start_scheduler()

//Initilize the scheduler system
void init_scheduler();
void start_scheduler();
