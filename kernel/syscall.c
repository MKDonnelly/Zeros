#include <kernel/syscall.h>

#include <arch/current_arch>
#include <kernel/sched/sched.h>
#include <fs/vfs/vnode.h>
#include <kernel/task.h>
#include <kernel/sched/sched.h>
#include <kernel/pipe.h>

//TODO remove
ktask_t *sys_spawn(char*,char*,int,int);

//Maps all system calls to their respective
//handlers
void syscalls_init(){
   arch_syscalls_init();

   arch_register_syscall( sys_open, 0 );
   arch_register_syscall( current_scheduler->scheduler_exit_task, 1 );
   arch_register_syscall( sys_write, 2 );
   arch_register_syscall( sys_getpid, 3 );
   arch_register_syscall( sys_read, 4 );
   arch_register_syscall( sys_getargs, 5 );

   arch_register_syscall( sys_spawn, 6 );

   arch_register_syscall( sys_createf, 7 );
   arch_register_syscall( sys_created, 8 );
   arch_register_syscall( sys_exists, 9 );

   arch_register_syscall( current_scheduler->scheduler_join_task, 10 );

   arch_register_syscall( sys_chdir, 11 );
   arch_register_syscall( sys_getpwd, 12 );
   arch_register_syscall( sys_readdir, 13 );

   arch_register_syscall( sys_makepipe, 14 );
   arch_register_syscall( sys_close, 15 );

   arch_register_syscall( sys_seek, 16 );
   arch_register_syscall( sys_delete, 17 );
}
