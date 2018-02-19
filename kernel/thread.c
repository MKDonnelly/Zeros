#include <kernel/thread.h>

//Internally used by k_create_thread to assign
//a unique id to every thread
int next_thread_id = 0;

kthread_t *k_create_thread ( void *start_func, void *param, void *exit_func, uint32_t stack_size ){

   //Create a thread descriptor
   kthread_t *thread = kmalloc(sizeof(kthread_t), KMALLOC_NO_ALIGN, 0 );

   //Allocate stack space for the thread
   //WE MUST ALIGN THE THREAD STACK TO 4K OR ELSE THE
   //THREADS WILL GET ASYMETRIC PROCESSING TIME!
   void *stack = kmalloc( stack_size, KMALLOC_ALIGN, 0 );

   thread->stack_ptr = stack;

   //Create a context for the thread
   arch_create_thread_context( &(thread->context), start_func, param,
                                exit_func, stack, stack_size );

   // Null the exit status
   thread->return_value = 0;

   //Add a unique thread id
   thread->thread_id = next_thread_id++;

   return thread;
}
