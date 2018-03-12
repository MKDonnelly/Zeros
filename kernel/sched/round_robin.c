#include <kernel/sched/round_robin.h>

struct sched_alg rr_alg = (struct sched_alg){
   .add_task       =   rr_add_task,
   .rm_task        =   rr_rm_task,
   .exit_task      =   rr_exit_task,
   .yield_task     =   rr_yield_task,
   .join_task      =   rr_join_task,
   .schedule       =   rr_schedule,
   .init_scheduler =   rr_init_scheduler
};

ktask_t *task_list = NULL;

ktask_t *current_task = NULL;
page_directory_t *current_page_dir = NULL;
int current_task_index = 0;

int task_count = 0;

void rr_add_task( ktask_t *new_task){
   add_node_ll( (void**)&task_list, new_task, task_count++);
}

int rm_task_helper(void *node, void *ref){
   if( (ktask_t*)node == (ktask_t*)ref )
      return 1;
   return 0;
}

void rr_rm_task(ktask_t *descriptor){
}

//We presume the current task calls yield
void rr_yield_task(){
   arch_trigger_interrupt( SCHEDULER_INTERRUPT );
}

//We presume that the current task is calling this if run
void rr_exit_task(void *return_value){
   current_task->state = TASK_EXIT;
   current_task->return_value = return_value;
   rr_yield_task();
}

static int jt_helper(void *node, void *ref){
   if( ((ktask_t*)node)->state == TASK_EXIT && (ktask_t*)node == (ktask_t*)ref )
      return 1;
   return 0;
}

void *rr_join_task(ktask_t *descriptor){

   ktask_t *task_to_join = NULL;
   while( task_to_join == NULL ){
      task_to_join = (ktask_t*)find_node_ll( (void**)&task_list, descriptor, jt_helper);
   }
   return task_to_join->return_value;
}

//Fork the calling task
/*
void scheduler_fork(){
   ktask_t *forked_task = k_malloc( kernel_heap, sizeof(ktask_t), 0 );
   forked_task->context = current_task->context;
   forked_task->stack_ptr = current_task->stack_ptr;
   forked_task->task_page_directory = clone_dir( current_task->task_page_directory );
   forked_task->state = TASK_READY;
   forked_task->task_id = 999;

   rr_add_task( forked_task );   
}*/

void idle_task(){
   while(1) arch_halt_cpu();
}

//Never returns
void rr_init_scheduler(){

   //Add idle task
   add_node_ll( (void**)&task_list, k_create_kernel_task(idle_task, NULL, NULL, 1024, kernel_page_dir), 0 );
   task_count++;   
   
   current_task = get_node_ll( (void**)&task_list, 0 );
   arch_jump_to_thread( current_task->context );
}

//The main scheduler. Routinely called to manage tasks
thread_context_t *rr_schedule(thread_context_t *interrupted_task){
   current_task->context = interrupted_task;

   do{
      current_task_index = (current_task_index + 1) % task_count;
      current_task = get_node_ll( (void**)&task_list, current_task_index);
   }while( current_task->state != TASK_READY );

   //Switch page dir if needed
   if( current_task->task_page_directory != current_page_dir ){
      current_page_dir = current_task->task_page_directory;
      load_page_dir( current_page_dir );
   }

   return current_task->context;
}
