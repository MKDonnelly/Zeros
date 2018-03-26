#include <kernel/task.h>

//Internally used by k_create_task to assign
//a unique id to every task
int next_task_id = 0;

ktask_t *k_create_kernel_task ( void *start_func, void *param, void *exit_func, uint32_t stack_size, page_directory_t *task_page_dir){

   //Create a thread descriptor
   ktask_t *new_task = k_malloc( kernel_heap, sizeof(ktask_t), 0 );

   //Allocate stack space for the thread
   //WE MUST ALIGN THE THREAD STACK TO PAGE_SIZE OR ELSE THE
   //THREADS WILL GET ASYMETRIC PROCESSING TIME DUE TO ALIGNMENT!
   void *stack = k_malloc( kernel_heap, stack_size, ARCH_PAGE_SIZE );

   new_task->stack_ptr = stack;

   //Create a context for the task
   if( exit_func == NULL )
      exit_func = current_sched_alg->exit_task;

   arch_create_kernel_context( &(new_task->context), start_func, param,
                                exit_func, stack, stack_size );

   //Add in page structure
   new_task->task_page_directory = task_page_dir;

   // Null the exit status
   new_task->return_value = 0;
   new_task->state = TASK_READY;

   //Add a unique task id
   new_task->task_id = next_task_id++;

   return new_task;
}

ktask_t *k_create_userland_task ( void *start_func, void *param, void *exit_func, uint32_t stack_size, uint32_t stack_addr, page_directory_t *task_page_dir){

   //Create a thread descriptor
   ktask_t *new_task = k_malloc( kernel_heap, sizeof(ktask_t), 0 );

   //Allocate stack space for the thread
   void *stack = (void*)stack_addr;

   //Used to free the memory allocated 
   new_task->stack_ptr = (void*)stack_addr;

   //Create a context for the task
   if( exit_func == NULL )
      exit_func = current_sched_alg->exit_task;

   arch_create_userland_context( &(new_task->context), start_func, param,
                                exit_func, stack, stack_size );

   //Add in page structure
   new_task->task_page_directory = task_page_dir;

   // Null the exit status
   new_task->return_value = 0;
   new_task->state = TASK_READY;

   //Add a unique task id
   new_task->task_id = next_task_id++;

   return new_task;
}
