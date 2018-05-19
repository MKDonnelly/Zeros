#pragma once

#include <arch/x86/archx86.h>

#define SYSCALL_INT 0x31
#define TOTAL_SYSCALLS 2

void arch_init_syscalls();
void arch_register_syscall( void *syscall_handler, int syscall_number );
