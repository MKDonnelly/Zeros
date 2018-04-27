#include <kernel/task.h>

//Internally used by k_create_task to assign
//a unique id to every task
int next_task_id = 0;

ktask_t *k_create_ktask( void (*start)(), void *param, 
                         void (*exit)(), uint32_t *stack){

   //Create a task descriptor
   ktask_t *new_task = k_malloc( kernel_heap, sizeof(ktask_t), 0 );

   //WE MUST ALIGN THE THREAD STACK TO PAGE_SIZE OR ELSE THE
   //THREADS WILL GET ASYMETRIC PROCESSING TIME DUE TO ALIGNMENT!
   new_task->task_stack = stack;

   //Intilize the arch-specific member task_info 
   new_task->task_info = arch_create_ktask(start, param, exit, stack);

   // Setup generic task info
   new_task->state = TASK_READY;
   new_task->ret_val = NULL;
   new_task->task_id = next_task_id++;
   new_task->is_kernel_task = 1;
   new_task->task_brk_len = 0;
   new_task->task_brk = NULL;

   return new_task;
}

ktask_t *k_create_utask( void (*start)(), void *param, 
                         void (*exit)(), uint32_t *stack){

   //Create a task descriptor
   ktask_t *new_task = k_malloc( kernel_heap, sizeof(ktask_t), 0 );

   new_task->task_stack = stack;

   //Create bare arch-specific task info
   new_task->task_info = arch_create_utask(start, param, exit, stack);

   //Initilize generic task info
   new_task->state = TASK_READY;
   new_task->ret_val = NULL;
   new_task->task_id = next_task_id++;
   new_task->is_kernel_task = 0;
   new_task->task_brk_len = 0;
   new_task->task_brk = NULL;

   return new_task;
}

ktask_t *k_create_utask_elf( char *elf_data ){ 

   //Create a task descriptor
   ktask_t *new_task = k_malloc( kernel_heap, sizeof(ktask_t), 0 );

   //Create bare arch-specific task info
   new_task->task_info = arch_load_utask_elf(elf_data);

   //Initilize generic task info
   new_task->state = TASK_READY;
   new_task->ret_val = NULL;
   new_task->task_id = next_task_id++;
   new_task->is_kernel_task = 0;
   new_task->task_brk_len = 0;
   new_task->task_brk = NULL;

   return new_task;
}
