#include <kernel/sched/round_robin.h>

#include <kernel/task.h>
#include <lib/generic_ll.h>
#include <lib/timing.h>


//Linked list of tasks to run
static ktask_t *task_list = NULL;

//Pointer to current task
static ktask_t *current_task = NULL;

//Number of tasks in the task list
static int task_count = 0;
int current_task_index = 0;

//Returns the current ktask.
ktask_t *rr_current_ktask(){
   return current_task;
}

//The main scheduler. Routinely called to manage tasks
void rr_schedule(){
   do{
      current_task_index = (current_task_index + 1) % task_count;
      current_task = list_get_node( task_list, current_task_index);
   }while( current_task->state != TASK_READY );

   arch_run_next( &(current_task->task_info) );
}


static void idle_task(){
   arch_enable_ints();
   while(1) arch_halt_cpu();
}

void rr_setup(){
   //Add idle task
   ktask_t *idle = ktask_create(idle_task, NULL, NULL);
   list_add( task_list, idle, 0 );
   task_count++;
}   

void rr_start(){
   //Call scheduler ever 100 ms
   timing_set_alarm( rr_schedule, 100 );
   
   //Register the scheduler function with the
   //scheduler interrupt
   arch_register_scheduler( rr_schedule );

   //Index 0 is for the idle thread
   current_task = list_get_node( task_list, 0 );
   arch_run_next( &current_task->task_info );
   idle_task();
}


void rr_add_task( ktask_t *new_task){
   list_add( task_list, new_task, task_count++);
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

void *rr_join_task(ktask_t *descriptor){
   return NULL;
}

//Fork the calling task
void scheduler_fork(){
}


schedalg_t rr_scheduler = {
   .scheduler_setup      = rr_setup,
   .scheduler_start      = rr_start,
   .scheduler_add_task   = rr_add_task,
   .scheduler_rm_task    = rr_rm_task,
   .scheduler_yield_task = rr_yield_task,
   .scheduler_exit_task  = rr_exit_task,
   .scheduler_join_task  = rr_join_task,
   .scheduler_schedule   = rr_schedule,
   .scheduler_current_ktask = rr_current_ktask,
};
