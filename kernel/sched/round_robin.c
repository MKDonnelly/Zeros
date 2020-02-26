#include <kernel/sched/round_robin.h>

#include <kernel/task.h>
#include <lib/genericll.h>
#include <lib/timing.h>

//Linked list of tasks to run
static list_t task_list = LIST_SINIT(__builtin_offsetof(ktask_t, ktask_list));

//Pointer to current task
static ktask_t *current_task = NULL;

//Returns the current ktask.
ktask_t *rr_current_ktask(){
   return current_task;
}

//The main scheduler. Routinely called to manage tasks
void rr_schedule(){
   do{
      current_task = list_next_node(&task_list, current_task, ktask_t);
      if( current_task == NULL )
         current_task = list_getfront(&task_list, ktask_t);
   }while( current_task->state != TASK_READY );

   arch_run_next( &(current_task->task_info) );
}


static void idle_task(){
   arch_enable_ints();
   while(1) arch_halt_cpu();
}

void rr_setup(){
}   

void rr_start(){
   //Call scheduler ever 100 ms
   timing_set_alarm( rr_schedule, 100 );
   
   //Register the scheduler function with the
   //scheduler interrupt
   arch_register_scheduler( rr_schedule );

   //Add idle task
   ktask_t *idle = ktask_create(idle_task, NULL, NULL);
   list_pushfront(&task_list, idle);

   current_task = idle;
   arch_run_next( &current_task->task_info );
   idle_task();
}


void rr_add_task( ktask_t *new_task){
   list_pushfront(&task_list, new_task);
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

   //Free all file descriptors
   for(int i = 0; i < 20; i++){
      if( current_task->open_fs[i] != NULL ){
         if( current_task->open_fs[i]->close != NULL ){
            current_task->open_fs[i]->close(current_task->open_fs[i]);
         }
      } 
   }

   rr_yield_task();
}

void *rr_join_task(ktask_t *descriptor){
   if( descriptor != NULL ){
      while( descriptor->state != TASK_EXIT ){
         rr_yield_task();
      }
   }
   return NULL;
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
