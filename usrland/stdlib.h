#pragma once

#define stdin 0
#define stdout 1
#define stderr 2

extern int do_syscall1(int syscall);
extern int do_syscall2(int syscall, ...);
extern int do_syscall3(int syscall, ...);
extern int do_syscall4(int syscall, ...);
extern int do_syscall5(int syscall, ...);

#define syscall_open(name) do_syscall2(0, name)
#define syscall_exit(status) do_syscall2(1, status)
#define syscall_write(fd, buf, len) do_syscall4(2, fd, buf, len)
#define syscall_getpid() do_syscall1(3)
#define syscall_read(fd, buf, len) do_syscall4(4, fd, buf, len)
#define syscall_getargs(buf, len) do_syscall3(5, buf, len)
#define syscall_spawn(bin, arg, stdinfd, stdoutfd) do_syscall5(6, bin, arg, stdinfd, stdoutfd)
#define syscall_createf(pdir_fd, name) do_syscall3(7, pdir_fd, name)
#define syscall_created(parent, child) do_syscall3(8, parent, child)
#define syscall_exists(name) do_sycall2(9, name)

#define syscall_join(task) do_syscall2(10, task)
#define syscall_chdir(dir) do_syscall2(11, dir)
#define syscall_getpwd(buf, len) do_syscall3(12, buf, len)
#define syscall_readdir(fd, buf, len, idx) do_syscall5(13, fd, buf, len, idx)
#define syscall_makepipe(fdin, fdout) do_syscall3(14, fdin, fdout)
#define syscall_close(fd) do_syscall2(15, fd)
#define syscall_seek(fd, offset, from) do_syscall4(16, fd, offset, from)
#define syscall_delete(name) do_syscall2(17, name)


//Utility functions
int mystrlen(char[]);
void mymemset(void*,int,int);
int mystrcmp(char*,char*);
void newline();
void myconcat(char*,char*,char*);
void mystrcpy(char*,char*);
int strsplit(char**,char*,char);
void makeapath(char*,char*);
