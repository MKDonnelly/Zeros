#include <arch/x86/pmode/context.h>

//Sets up a context for a kernel-level task and returns a pointer
//to the head of the stack (a context_t* type)
context_t *arch_create_kcontext( void (*start)(void *), void *param, 
                                 void (*exit)(), uint32_t *stack_addr ){

   /*    Kernel context layout 
      stack_addr+0x0: 1st parameter
      stack_addr+0x4: exit function
      stack_addr+   : context_t struct
   */

   //The first item on the stack is the first parameter
   stack_addr--;
   *stack_addr = (uint32_t)param;
  
   //The next item is the address of the function to call
   //upon return. 
   stack_addr--;
   *stack_addr = (uint32_t)exit;

   //The next is the set of registers that will be popped when
   //this task begins. This is of type context_t
   context_t *context = (context_t*)((uint8_t*)stack_addr - sizeof(context_t));
   context->eip = (uint32_t)start;
   context->eflags = INITIAL_EFLAGS;

   //This is a kernel task, so we use the kernel code segment for cs
   context->cs = gdt_kernel_code;
   context->esp_pushed = context->ebp = (uint32_t)context;

   //Kernel level tasks used the kernel data gdt descriptor
   context->ss = context->gs = context->fs =
      context->es = context->ds = gdt_kernel_data;

   //Return the head of the stack, which will point to a context_t
   return context;
}


context_t *arch_create_ucontext( void (*start)(void*), void *param, 
                                void (*exit)(), uint32_t *stack_addr ){

   /*    User context layout 
      stack_addr+0x0: 1st parameter
      stack_addr+0x4: exit function
      stack_addr+   : context_t struct
   */

   //Put the first parameter on the stack
   stack_addr--;
   *stack_addr = (uint32_t)param;
   
   //Put the exit function on the stack
   stack_addr--;
   *stack_addr = (uint32_t)exit;

   //Put the context_t on the stack
   context_t *context = (context_t*)((uint8_t*)stack_addr - sizeof(context_t));

   context->eip = (uint32_t)start;
   context->cs = 0x1B;
   context->ss = 0x23;
   context->eflags = INITIAL_EFLAGS;
   context->ebp = context->esp_pushed = (uint32_t)context;

   //Return a pointer to the head of the stack of type context_t
   return context;
}

