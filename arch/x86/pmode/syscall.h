#pragma once

#define SYSCALL_INT 0x31
#define TOTAL_SYSCALLS 2

void arch_syscalls_init();
void arch_register_syscall( void *syscall_handler, int syscall_number );
