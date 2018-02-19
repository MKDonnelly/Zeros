#include <arch/x86/context.h>

//A thread context is identical to the interrupt stack layout
void arch_create_thread_context (thread_context_t **context, 
   void (*func)(void *), void *param, void (*thread_exit)(), void *stack, uint32_t stack_size ){

        //Pointer to the head of the thread's stack
        uint32_t *thread_stack = stack + stack_size;

        // Give the first parameter to the thread by placing
        // it at the head of the thread stack
        thread_stack--;
        *thread_stack = (uint32_t)param;

        //This is called when the function returns
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
void arch_jump_to_thread(thread_context_t *to){
   asm volatile("      \
      movl %0, %%esp;  \
      popal;          \
      add $8, %%esp;   \
      ret;             "
      : : "m" (to));
}
