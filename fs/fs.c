#include <fs/fs.h>
#include <kernel/sched/sched.h>
#include <arch/current_arch>
#include <kernel/task.h>
#include <lib/print.h>
#include <arch/current_arch>

fs_node_t *fs_root = NULL;

//System call
void sys_open(int fd, char *addr){
   k_printf("Open called!\n");
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();

   char buf[30];
   vm_copy_from_pdir( (uint32_t)addr, ctask->task_info.task_pd, buf, 5);
   k_printf("%c%c%c%c%c\n", buf[0], buf[1], buf[2], buf[3], buf[4]); 
}

void sys_write(int fd, char *ubuf, int len){
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();

   char kbuf[30];
   vm_copy_from_pdir( (uint32_t)ubuf, ctask->task_info.task_pd, kbuf, 5 );
   kbuf[5] = 0;
   k_printf("Stdout: %s\n", kbuf);
}


uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, int8_t *buffer){
   if( node->read != NULL )
      return node->read(node, offset, size, buffer);
   else
      return 0;
}

uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, int8_t *buffer){
   if( node->write != NULL )
      return node->write(node, offset, size, buffer);
   else
      return 0;
}

void open_fs(fs_node_t *node, uint8_t read, uint8_t write){
   if( node->open != NULL )
      node->open(node);
}

void close_fs(fs_node_t *node){
   if( node->close != NULL )
      node->close(node);
}


fs_node_t *readdir_fs(fs_node_t *node, uint32_t index){
   if( node->readdir != NULL && (node->flags & FS_DIRECTORY))
      return node->readdir(node, index);
   else
      return NULL;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name){
   if( node->finddir != NULL && (node->flags & FS_DIRECTORY))
      return node->finddir(node, name);
   else
      return NULL;
}
