#include "syscall.h"

SYSCALL1(putchar, 0, char);
SYSCALL1(exit, 1, void*);

static void syscall_handler(registers_t regs);

static void *syscalls[] = { k_putchar, rr_exit_task };

void init_syscalls(){

   //Reserve space for systemcall stack
   char *syscall_stack = k_malloc( kernel_heap, 0x1000, 0 );
   set_kernel_stack( (uint32_t)syscall_stack );

   arch_register_interrupt(0x50, syscall_handler );
}

void syscall_handler(registers_t regs){

   if( regs.eax != 0 && regs.eax != 1)
       return;


   void *location = syscalls[regs.eax];
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

