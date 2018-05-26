#include <arch/x86/pmode/arch_task.h>

#include <arch/x86/pmode/paging.h>
#include <arch/x86/pmode/loader.h>
#include <lib/assert.h>
#include <kernel/mm/heap.h>
#include <arch/x86/frame.h>

arch_task_t arch_ktask_create(void (*start)(), void *param,
                                   void (*exit)(), uint32_t *stack){
   KASSERT( start != NULL );
   KASSERT( stack != NULL );

   arch_task_t kernel_task;

   //Create the context
   kernel_task.task_context = 
                arch_kcontext_create(start, param, exit, stack);
   
   //Kernel tasks will always inherit from kernel_page_dir
   kernel_task.task_pd = kernel_page_dir;

   //No interrupt stack needed for system calls since this is a
   //kernel-level stack
   kernel_task.interrupt_stack = NULL;

   return kernel_task;
}

//NOTE: This is used to create a userland task from a function in
//the kernel code. This is for testing only, since all userland
//programs will be loaded from and ELF file.
arch_task_t arch_utask_create(void (*start)(), void *param,
                                   void (*exit)(), uint32_t *stack){
   KASSERT( start != NULL );
   KASSERT( stack != NULL );

   arch_task_t user_task;

   //Create context
   user_task.task_context = arch_ucontext_create(start, param, exit, stack);

   //Userland tasks will clone the page directory of kernel_page_dir
   user_task.task_pd = vm_pdir_clone( kernel_page_dir );

   //Setup a system call stack of 1KB. Align on page boundary
   user_task.interrupt_stack = k_malloc( 1024, 0x1000);

   return user_task;
}

#define USERLAND_STACK 0xB0000000

arch_task_t arch_utask_from_elf( char *elf_file_buffer ){
   KASSERT(elf_file_buffer != NULL);

   arch_task_t user_task;

   //Userland task will inherit the pd from kernel_page_dir
   user_task.task_pd = vm_pdir_clone( kernel_page_dir );

   uint32_t start_addr = arch_create_from_elf( (Elf32_Ehdr*)elf_file_buffer, user_task.task_pd);

   //Map in a stack for the task
   uint32_t ustack = framepool_first_free();
   //Subtract the size of the page, since the stack head will be AT
   //USERLAND_STACK.
   vm_pmap( USERLAND_STACK - ARCH_PAGE_SIZE, ustack, user_task.task_pd, 1, 1 );
   vm_pmap_temp( USERLAND_STACK - ARCH_PAGE_SIZE, ustack, kernel_page_dir );

   //Create context. In this case, we do not need to subtract 
   //ARCH_PAGE_SIZE since arch_create_ucontext assumes that is is
   //passed the head of the stack.
   user_task.task_context = arch_ucontext_create((void*)start_addr, 
                               NULL, NULL, (uint32_t*)USERLAND_STACK );

   //Setup a syscall stack of 1K aligned on page boundary
   user_task.interrupt_stack = k_malloc( 1024, 0x1000 );
   KASSERT( user_task.interrupt_stack != NULL );

   return user_task;
}
