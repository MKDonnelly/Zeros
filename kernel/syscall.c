#include <kernel/syscall.h>

#include <arch/current_arch>
#include <kernel/sched/sched.h>
#include <fs/vfs/fs.h>
#include <kernel/task.h>
#include <kernel/sched/sched.h>

//Maps all system calls to their respective
//handlers
void syscalls_init(){
   arch_syscalls_init();

   arch_register_syscall( sys_open, 0 );
   arch_register_syscall( current_scheduler->scheduler_exit_task, 1 );
   arch_register_syscall( sys_write, 2 );
   arch_register_syscall( sys_getpid, 3 );
}
