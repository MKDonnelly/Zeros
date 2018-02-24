#pragma once

#include <arch/x86/archx86.h>

void init_syscalls();

#define DECL_SYSCALL0(fn) int syscall##fn();
#define DECL_SYSCALL1(fn,p1) int syscall##fn(p1);
#define DECL_SYSCALL2(fn,p1,p2) int syscall##fn(p1,p2);
#define DECL_SYSCALL3(fn,p1,p2,p3) int syscall##fn(p1,p2,p3);
#define DECL_SYSCALL4(fn,p1,p2,p3,p4) int syscall##fn(p1,p2,p3,p4);
#define DECL_SYSCALL5(fn,p1,p2,p3,p4,p5) int syscall##fn(p1,p2,p3,p4,p5);

#define DEFN_SYSCALL0(fn, num) \
int syscall_##fn()\
{ \
   int a; \
   asm volatile("int $0x80" : "=a" (a) : "0" (num)); \
   return a;   \
}

#define DEFN_SYSCALL1(fn, num, Type1) \
int syscall_##fn(Type1 p1) \
{\
   int a;  \
   asm volatile("int $80" : "=a" (a) : "0" (num), "b"((int)p1)); \
   return a; \
}

#define DEFN_SYSCALL2(fn, num, Type1, Type2) \
int syscall_##fn(Type1 p1, Type2 p2) \
{\
   int a;  \
   asm volatile("int $0x80" : "=a" (a) : "0" (num), "b"((int)p1), "c"((int)p2)); \
   return a; \
}

#define DEFN_SYSCALL3(fn, num, Type1, Type2, Type3) \
int syscall_##fn(Type1 p1, Type2 p2, Type3 p3) \
{\
   int a;  \
   asm volatile("int $0x80" : "=a" (a) : "0" (num), "b"((int)p1), "c"((int)p2), "d"((int)p3)); \
   return a; \
}



DECL_SYSCALL1(k_putchar, char)
