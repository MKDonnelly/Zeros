#include "syscall.h"

#include <arch/x86/pmode/isr.h>

extern context_t *get_current_context();
//TODO add in a struct with a void* to the function
//     and the number of arguments the function takes.
static uint32_t (*syscall_table[TOTAL_SYSCALLS])();

//All system calls are routed here
static void syscall_handler(){

   context_t *current_context = get_current_context();

   if( current_context->eax < 0 || current_context->eax > TOTAL_SYSCALLS )
      return;

   uint32_t (*syscall)() = syscall_table[current_context->eax];
   current_context->eax = syscall(current_context->ebx, 
                                  current_context->ecx, 
                                  current_context->edx, 
                                  current_context->esi, 
                                  current_context->edi);
}


//Initilizes the system call interrupt
void arch_syscalls_init(){
   arch_register_interrupt(SYSCALL_INT, syscall_handler );
}

void arch_register_syscall( void *syscall_handler, int syscall_number ){
   if( syscall_number >= 0 && syscall_number < TOTAL_SYSCALLS )
      syscall_table[syscall_number] = syscall_handler;
}

