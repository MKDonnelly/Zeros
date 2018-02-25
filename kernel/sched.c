#include <kernel/sched.h>

struct sched_alg current_sched_alg;
thread_context_t *cur_context = NULL;

void scheduler_handler(){
   cur_context = current_sched_alg.schedule( cur_context );   
}

void init_threading(struct sched_alg alg){
  
   current_sched_alg = alg;
   arch_register_interrupt( SCHEDULER_INTERRUPT, scheduler_handler);
   current_sched_alg.init_scheduler();
}
