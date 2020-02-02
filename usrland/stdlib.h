#pragma once

#define stdin 0
#define stdout 1
#define stderr 2

extern int do_syscall1(int syscall, ...);
extern int do_syscall2(int syscall, ...);
extern int do_syscall3(int syscall, ...);
extern int do_syscall4(int syscall, ...);
extern int do_syscall5(int syscall, ...);

#define syscall_open(name) do_syscall2(0, name)
#define syscall_exit(status) do_syscall2(1, status)
#define syscall_write(fd, buf, len) do_syscall4(2, fd, buf, len)
#define syscall_getpid() do_syscall1(3)
#define syscall_read(fd, buf, len) do_syscall4(4, fd, buf, len)
#define syscall_test() do_syscall1(5)
