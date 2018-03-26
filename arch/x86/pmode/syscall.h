#pragma once

#include <arch/x86/archx86.h>
#include <kernel/mm/heap.h>

#define SYSCALL_INT 0x31
#define TOTAL_SYSCALLS 1

void init_syscalls();
void register_syscall( void (*syscall)(), int syscall_number );
void unregister_sycall( int syscall_number );


/*
#define SYSCALL0(fn, syscall_number) \
int syscall_##fn() { \
   int syscall_return;   \
   asm volatile("int $0x31" : "=a" (syscall_return) : "0" (syscall_number)); \
   return syscall_return; \
}

#define SYSCALL1(fn, syscall_number, Type1) \
int syscall_##fn(Type1 arg1) { \
   int syscall_return;   \
   asm volatile("int $0x31" : "=a" (syscall_return) : "0" (syscall_number), "b"((int)arg1)); \
   return syscall_return; \
}*/

//System calls available to userland programs
//int syscall_putchar(char);
//int syscall_exit(void*);
