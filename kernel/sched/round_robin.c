#include <kernel/sched/round_robin.h>

//Linked list of tasks to run
static ktask_t *task_list = NULL;

//Pointer to current task
static ktask_t *current_task = NULL;

//Number of tasks in the task list
static int task_count = 0;
int current_task_index = 0;


//The main scheduler. Routinely called to manage tasks
void rr_schedule(){
   do{
      current_task_index = (current_task_index + 1) % task_count;
      current_task = get_node_ll( task_list, current_task_index);
   }while( current_task->state != TASK_READY );

   arch_run_next( &(current_task->task_info) );
}


static void idle_task(){
   arch_enable_ints();
   while(1) arch_halt_cpu();
}

void rr_setup(){
   //Add idle task
//   char *idle_stack = k_malloc( 2000, 0x1000);
//   ktask_t *idle = k_create_ktask(idle_task, NULL, NULL, STACK_HEAD(idle_stack, 2000 ));
   ktask_t *idle = k_create_ktask(idle_task, NULL, NULL);
   add_node_ll( task_list, idle, 0 );
   task_count++;
}   

void rr_start(){
   //Index 0 is for the idle thread
   current_task = get_node_ll( task_list, 0 );
   arch_run_next( &current_task->task_info );
   idle_task();
}


void rr_add_task( ktask_t *new_task){
   add_node_ll( task_list, new_task, task_count++);
}


void rr_rm_task(ktask_t *descriptor){
}

//We presume the current task calls yield
void rr_yield_task(){
   arch_trigger_scheduler();
}

//We presume that the current task is calling this if run
void rr_exit_task(void *return_value){
   current_task->state = TASK_EXIT;
   current_task->ret_val = return_value;
   rr_yield_task();
}
/*
static int jt_helper(void *node, void *ref){
   if( ((ktask_t*)node)->state == TASK_EXIT && (ktask_t*)node == (ktask_t*)ref )
      return 1;
   return 0;
}*/

void *rr_join_task(ktask_t *descriptor){
/*
   ktask_t *task_to_join = NULL;
   while( task_to_join == NULL ){
      task_to_join = (ktask_t*)find_node_ll( (void**)&task_list, descriptor, jt_helper);
   }
   return task_to_join->return_value;*/
   return NULL;
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




schedalg_t rr_scheduler = {
   .scheduler_setup      = rr_setup,
   .scheduler_start      = rr_start,
   .scheduler_add_task   = rr_add_task,
   .scheduler_rm_task    = rr_rm_task,
   .scheduler_yield_task = rr_yield_task,
   .scheduler_exit_task  = rr_exit_task,
   .scheduler_join_task  = rr_join_task,
   .scheduler_schedule   = rr_schedule
};

