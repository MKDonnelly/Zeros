#include <staging/round_robin.h>

struct sched_alg rr_alg = (struct sched_alg){
   .add_thread = rr_add_thread,
   .rm_thread  = rr_rm_thread,
   .exit_thread = rr_exit_thread,
   .yield_thread = rr_yield_thread,
   .join_thread = rr_join_thread,
   .schedule = rr_schedule,
   .init_scheduler = rr_init_scheduler
};

kthread_t *thread_list = NULL;

kthread_t *current_thread = NULL;
int current_thread_index = 0;

int thread_count = 0;

void rr_add_thread( kthread_t *thread){
   add_node_ll( (void**)&thread_list, thread, thread_count++);
}


void idle_thread(){
   while(1) arch_halt_cpu();
}

void rr_init_scheduler(){

   //Add idle thread
   add_node_ll( (void**)&thread_list, k_create_thread(idle_thread, NULL, NULL, 1024), 0 );
   thread_count++;   
   
   current_thread = get_node_ll( (void**)thread_list, 0 );
   arch_enable_ints();
   arch_jump_to_thread( current_thread->context );
}


thread_context_t *rr_schedule(thread_context_t *interrupted_thread){
   current_thread->context = interrupted_thread;

   do{
      current_thread_index = (current_thread_index + 1) % thread_count;
      current_thread = get_node_ll( (void**)&thread_list, current_thread_index);
   }while( current_thread->state == THREAD_EXIT );

   return current_thread->context;

}

void rr_rm_thread(kthread_t *k){};
void rr_exit_thread(){};
void rr_yield_thread(){};
void rr_join_thread(){};

