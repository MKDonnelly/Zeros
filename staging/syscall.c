#include "syscall.h"

//DEFN_SYSCALL1(k_putchar, 0, char);
SYSCALL1(putchar, 0, char);

static void syscall_handler(registers_t regs);

static void *syscalls[] = { k_putchar };

void init_syscalls(){
   arch_register_interrupt(0x50, syscall_handler );
   //arch_register_interrupt(0x50, syscall_handler );
}

void syscall_handler(registers_t regs){

   if( regs.eax != 0 )
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

