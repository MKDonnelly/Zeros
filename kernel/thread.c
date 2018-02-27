#include <kernel/thread.h>

//Internally used by k_create_thread to assign
//a unique id to every thread
int next_thread_id = 0;

kthread_t *k_create_thread ( void *start_func, void *param, void *exit_func, uint32_t stack_size ){

   //Create a thread descriptor
   kthread_t *thread = k_malloc( kernel_heap, sizeof(kthread_t), 0, 0 );

   //Allocate stack space for the thread
   //WE MUST ALIGN THE THREAD STACK TO PAGE_SIZE OR ELSE THE
   //THREADS WILL GET ASYMETRIC PROCESSING TIME!
   void *stack = k_malloc( kernel_heap, stack_size, 0x1000, 0 );

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

//Called by a thread to voluntarily
//give up processor time. 
void thread_yield(){
   //arch_trigger_interrupt( SCHEDULER_INTERRUPT );
}


//Called by a thread when returning
//Explicitly calling this will give the return
//value. If this is implicitly called when the
//thread index (when k_create_thread sets it up)
//the return value will be undefined.
void thread_exit(void *retval){/*
   current_thread->state = THREAD_EXIT;
   current_thread->return_value = retval;
   thread_yield();*/
}

