#include "syscall.h"

static void syscall_handler(context_t regs);
static uint32_t (*syscall_table[TOTAL_SYSCALLS])();

//All system calls are routed here
static void syscall_handler(context_t regs){

   if( regs.eax < 0 || regs.eax > TOTAL_SYSCALLS )
      return;

   uint32_t (*syscall)() = syscall_table[regs.eax];
   regs.eax = syscall(regs.ebx, regs.ecx, regs.edx, regs.esi, regs.edi);
}

//Initilizes the system call interrupt
void arch_init_syscalls(){
   arch_register_interrupt(SYSCALL_INT, syscall_handler );
}

void arch_register_syscall( void *syscall_handler, int syscall_number ){
   if( syscall_number >= 0 && syscall_number < TOTAL_SYSCALLS )
      syscall_table[syscall_number] = syscall_handler;
}

