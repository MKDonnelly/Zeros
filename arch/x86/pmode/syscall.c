#include "syscall.h"

static void syscall_handler(registers_t regs);

static void *syscall_table[TOTAL_SYSCALLS];

void init_syscalls(){

   arch_register_interrupt(SYSCALL_INT, syscall_handler );
}

void register_syscall( void (*syscall)(), int syscall_number ){
   if( syscall_number >= 0 && syscall_number < TOTAL_SYSCALLS )
      syscall_table[syscall_number] = syscall;
}


void syscall_handler(registers_t regs){

   if( regs.eax < 0 || regs.eax > TOTAL_SYSCALLS )
      return;

   void *location = syscall_table[regs.eax];
   int ret;
   asm volatile("   \
      push %1;      \
      push %2;      \
      push %3;      \
      push %4;      \
      push %5;      \
      call *%6;     \
      pop %%ebx;    \
      pop %%ebx;    \
      pop %%ebx;    \
      pop %%ebx;    \
      pop %%ebx;" : "=a" (ret) : "r" (regs.edi), "r"(regs.esi), "r"(regs.edx), "r"(regs.ecx), "r"(regs.ebx), "r"(location));
}

