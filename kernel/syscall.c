#include <kernel/syscall.h>

//Maps all system calls to their respective
//handlers
void init_syscalls(){
   arch_init_syscalls();

   arch_register_syscall( k_putchar, 0 );
   arch_register_syscall( current_scheduler->scheduler_exit_task, 1 );

   /*
      0x0 = sys_open
      0x1 = sys_read
      0x2 = sys_write
      0x3 = sys_close
   */
}


