#include <arch/x86/pmode/arch_task.h>


arch_task_info_t arch_create_ktask(void (*start)(), void *param,
                                   void (*exit)(), uint32_t *stack){

   arch_task_info_t kernel_task;

   //Create the context
   kernel_task.task_context = arch_create_kcontext( start, param, exit, stack);
   
   //Kernel tasks will always inherit from kernel_page_dir
   kernel_task.task_pd = kernel_page_dir;

   //No interrupt stack needed for system calls since this is a
   //kernel-level stack
   kernel_task.interrupt_stack = NULL;

   return kernel_task;
}

arch_task_info_t arch_create_utask(void (*start)(), void *param,
                                   void (*exit)(), uint32_t *stack){
   
   arch_task_info_t user_task;

   //Create context
   user_task.task_context = arch_create_ucontext(start, param, exit, stack);

   //Userland tasks will clone the page directory of kernel_page_dir
   user_task.task_pd = clone_pd( kernel_page_dir );

   //Setup a system call stack of 1KB. Align on page boundary
   user_task.interrupt_stack = k_malloc( kernel_heap, 1024, 0x1000);

   return user_task;
}
