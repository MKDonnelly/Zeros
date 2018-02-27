#include <kernel/sched.h>

struct sched_alg current_sched_alg;
thread_context_t *cur_context = NULL;

void scheduler_handler(){
   cur_context = current_sched_alg.schedule( cur_context );   
}

//Used to setup the threading system without actually starting
//the scheduler. This is meant to be called by kmain, which can
//then create threads to be run before starting the scheduler.
void setup_threading(struct sched_alg alg){
   current_sched_alg = alg;
   arch_register_interrupt( SCHEDULER_INTERRUPT, scheduler_handler);
   timing_set_alarm( scheduler_handler, 100 );
}

//This never returns
void start_scheduler(){
   current_sched_alg.init_scheduler();
}
