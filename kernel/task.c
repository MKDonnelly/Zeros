#include <kernel/task.h>

#include <kernel/mm/heap.h>
#include <lib/assert.h>
#include <kernel/sched/sched.h>

//Internally used by k_create_task to assign
//a unique id to every task
static int next_task_id = 0;

int sys_getpid(){
   return (current_scheduler->scheduler_current_ktask())->task_id;
}

#define STACK_SIZE 0x1000

ktask_t *ktask_create( void (*start)(), void *param, void (*exit)()){

   KASSERT( start != NULL );

   //Create a thread descriptor
   ktask_t *new_task = k_malloc( sizeof(ktask_t), 0 );
   KASSERT( new_task != NULL );

   //WE MUST ALIGN THE THREAD STACK TO PAGE_SIZE OR ELSE THE
   //THREADS WILL GET ASYMETRIC PROCESSING TIME DUE TO ALIGNMENT!
   //new_task->task_stack = stack;
   uint32_t *stack = k_malloc( STACK_SIZE, ARCH_PAGE_SIZE );
   KASSERT( stack != NULL );
   new_task->task_stack = STACK_HEAD( stack, ARCH_PAGE_SIZE );

   //Intilize the arch-specific member task_info 
   new_task->task_info = arch_ktask_create(start, param, exit, stack);

   // Setup generic task info
   new_task->state = TASK_READY;
   new_task->ret_val = NULL;
   new_task->task_id = next_task_id++;
   new_task->is_kernel_task = 1;
   new_task->task_brk_len = 0;
   new_task->task_brk = NULL;

   return new_task;
}

ktask_t *utask_create( void (*start)(), void *param, 
                         void (*exit)(), uint32_t *stack){

   KASSERT( start != NULL );
   KASSERT( stack != NULL && (uint32_t)stack < KERNEL_VADDR );
   KASSERT( IS_ALIGNED_ON( (uint32_t)stack, ARCH_PAGE_SIZE ) );

   //Create a thread descriptor
   ktask_t *new_task = k_malloc( sizeof(ktask_t), 0 );
   KASSERT( new_task != NULL );
   new_task->task_stack = stack;

   //Create bare arch-specific task info
   new_task->task_info = arch_utask_create(start, param, exit, stack);

   //Initilize generic task info
   new_task->state = TASK_READY;
   new_task->ret_val = NULL;
   new_task->task_id = next_task_id++;
   new_task->is_kernel_task = 0;
   new_task->task_brk_len = 0;
   new_task->task_brk = NULL;

   return new_task;
}

ktask_t *utask_from_elf( char *elf_data ){ 

   KASSERT( elf_data != NULL );

   //Create a task descriptor
   ktask_t *new_task = k_malloc( sizeof(ktask_t), 0 );
   KASSERT( new_task != NULL );

   //Create bare arch-specific task info
   new_task->task_info = arch_utask_from_elf(elf_data);

   //Initilize generic task info
   new_task->state = TASK_READY;
   new_task->ret_val = NULL;
   new_task->task_id = next_task_id++;
   new_task->is_kernel_task = 0;
   new_task->task_brk_len = 0;
   new_task->task_brk = NULL;

   return new_task;
}
