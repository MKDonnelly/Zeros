#include <arch/x86/pmode/spinlock.h>
#include <fs/vfs/vnode.h>

#include <kernel/task.h>
#include <kernel/sched/sched.h>

//Pipes are buffers that can hold 1K bytes
typedef struct pipe{
   mutex_t lock;
  
   int producer_closed;
   int consumer_closed;

   int data_len; 
   char data[1024];
}pipe_t;

void pipe_create(fs_node_t **fdin, fs_node_t **fdout);
//offset not used
int pipe_write(fs_node_t *pipe, int offset, int len, char *buf);
//offset not used
int pipe_read(fs_node_t *pipe, int offset, int len, char *buf);
int pipe_close(fs_node_t *pipe);

void sys_makepipe(int *fdin, int *fdout);
