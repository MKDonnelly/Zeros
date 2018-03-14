#include <kernel/sched/sched.h>

//Pointer to the specific scheduler algorithm
//being used. This provides and interface for the
//rest of the sytem to use
struct sched_alg *current_sched_alg;

//Used by low-level interrupt code to pass the
//current context to this level.
thread_context_t *cur_context = NULL;

void scheduler_handler(){
   cur_context = current_sched_alg->schedule( cur_context );   
}

//Used to setup the scheduling system without actually starting
//the scheduler. This is meant to be called by kmain, which can
//then create tasks to be run before starting the scheduler.
//This must be in place before we can do anything with the scheduler.
void init_scheduler(struct sched_alg *alg){
   //Set the scheduler algorithm passes as the current scheduler
   current_sched_alg = alg;

   //Let the scheduler set itself up
   current_sched_alg->init_scheduler();
}

//This never returns
void start_scheduler(){
   //This will be used when a task yields. Otherwise, scheduler_handler
   //will periodically be called by the timer set below.
   arch_register_interrupt( SCHEDULER_INTERRUPT, scheduler_handler);

   //Run scheduler_handler every 100ms
   timing_set_alarm( scheduler_handler, 100 );

   //Now actually start the scheduler
   k_start_scheduler();
}
