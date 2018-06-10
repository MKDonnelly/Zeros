#include <arch/x86/pmode/context.h>

#include <arch/x86/pmode/gdt.h>

//Sets up a context for a kernel-level task and returns a pointer
//to the head of the stack (a context_t* type)
context_t *arch_kcontext_create( void (*start)(void *), void *param, 
                                 void (*exit)(), size_t stack ){

   /*    Kernel context layout 
      stack_addr+0x0: 1st parameter
      stack_addr+0x4: exit function
      stack_addr+   : context_t struct
   */
   uint32_t *stack_addr = (uint32_t*)stack;

   //The first item on the stack is the first parameter
   stack_addr--;
   *stack_addr = (uint32_t)param;
  
   //The next item is the address of the function to call
   //upon return. 
   stack_addr--;
   *stack_addr = (uint32_t)exit;

   //The next is the set of registers that will be popped when
   //this task begins. This is of type context_t. We add 8 to make
   //a small adjustment to the interrupt frame: when interrupting to
   //the same privilege level (in this case, ring 0), the esp_pushed and
   //ss fields of the context_t ARE NOT USED. We skip over them by adding 8
   //(2 X 4 byte members=8) and not setting either esp_pushed or ss below.
   context_t *context = (context_t*)((char*)stack_addr 
                                     - sizeof(context_t) + 8);
   context->eip = (uint32_t)start;
   context->eflags = INITIAL_EFLAGS;

   //This is a kernel task, so we use the kernel code segment for cs
   context->cs = gdt_kernel_code;
   context->ebp = (uint32_t)context;

   //Kernel level tasks used the kernel data gdt descriptor
   context->gs = context->fs =
                 context->es = context->ds = gdt_kernel_data;

   //Return the head of the stack, which will point to a context_t
   return context;
}


context_t *arch_ucontext_create( void (*start)(void*), void *param, 
                                void (*exit)(), size_t stack ){

   /*    User context layout 
      stack_addr+0x0: 1st parameter
      stack_addr+0x4: exit function
      stack_addr+   : context_t struct
   */

   uint32_t *stack_addr = (uint32_t*)stack;

   //Put the first parameter on the stack
   stack_addr--;
   *stack_addr = (uint32_t)param;
   
   //Put the exit function on the stack
   stack_addr--;
   *stack_addr = (uint32_t)exit;

   //Put the context_t on the stack
   context_t *context = (context_t*)((char*)stack_addr- sizeof(context_t));

   context->eip = (uint32_t)start;
   //The gdt selectors for userland code/data, with 
   //the requestor's privilege level (0b11 = ring 3) ored
   //onto the end.
   context->cs = gdt_userland_code_rpl;
   context->ss = gdt_userland_data_rpl;
   context->eflags = INITIAL_EFLAGS;
   context->ebp = context->esp_pushed = (uint32_t)context;

   //Return a pointer to the head of the stack of type context_t
   return context;
}
