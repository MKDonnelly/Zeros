#pragma once

#include <arch/x86/archx86.h>
#include <kernel/sched/round_robin.h>

#define SYSCALL_INT 0x50

void init_syscalls();

#define SYSCALL0(fn, syscall_number) \
int syscall_##fn() { \
   int syscall_return;   \
   asm volatile("int $0x50" : "=a" (syscall_return) : "0" (syscall_number)); \
   return syscall_return; \
}

#define SYSCALL1(fn, syscall_number, Type1) \
int syscall_##fn(Type1 arg1) { \
   int syscall_return;   \
   asm volatile("int $0x50" : "=a" (syscall_return) : "0" (syscall_number), "b"((int)arg1)); \
   return syscall_return; \
}

//System calls available to userland programs
int syscall_putchar(char);
int syscall_exit(void*);
