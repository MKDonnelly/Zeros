#include <kernel/thread.h>


//A thread context is identical to the interrupt stack layout
void create_thread_context (thread_context_t **context, void (func) (void *), void *param, void (*thread_exit)(), void *stack, uint32_t stack_size ){

        //Pointer to the head of the thread's stack
        uint32_t *thread_stack = stack + stack_size;

        // Give the first parameter to the thread by placing
        // it at the head of the thread stack
        thread_stack--;
        *thread_stack = (uint32_t)param;

        // The second parameter on the stack will be the
        // function to call when exiting
        thread_stack--;
        *thread_stack = (uint32_t)thread_exit;

        // Add the thread context to the thread stack 
        // This really just allocates space for the registers;
        // the actual values are placed when we do a thread switch
        *context = (void *)thread_stack - sizeof(thread_context_t);

        // Initilize the thread context by setting the function
        // to call and the initial eflags value
        (*context)->eip = (uint32_t)func;
        (*context)->eflags = INITIAL_EFLAGS;
        (*context)->cs = gdt_kernel_code;
        (*context)->ebp = (uint32_t)(*context);
        (*context)->esp = (*context)->ebp;
}

//Called when first initilizing threading. 
void jump_to_thread(thread_context_t *to){
   asm volatile("      \
      movl %0, %%esp;  \
      popal;          \
      add $8, %%esp;   \
      ret;             "
      : : "m" (to));
}

kthread_t *k_create_thread ( void *start_func, void *param, void *exit_func, uint32_t stack_size, int run ){

   //Create a thread descriptor
   kthread_t *thread = kmalloc(sizeof(kthread_t), 0, 0 );

   //Allocate stack space for the thread
   void *stack = kmalloc( stack_size, 0, 0 );

   thread->stack_ptr = stack;

   //Create a context for the thread
   create_thread_context( &(thread->context), start_func, param,
                                exit_func, stack, stack_size );

   // Null the exit status
   thread->exit_status = 0;

   return thread;
}

