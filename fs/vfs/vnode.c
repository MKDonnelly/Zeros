#include <fs/vfs/vnode.h>
#include <kernel/sched/sched.h>
#include <arch/current_arch>
#include <kernel/task.h>
#include <lib/print.h>
#include <kernel/mm/heap.h>
#include <arch/current_arch>

fs_node_t *fs_root = NULL;

//System call
extern fs_node_t *root_fs;
int sys_open(char *name){
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();

   char buf[30];
   vm_copy_str((uint32_t)name, ctask->task_info.task_pd, buf, 30);
   k_printf("\nOpening file: %s\n", buf);

   fs_node_t *found_fs = root_fs->finddir(root_fs, buf);
   if( found_fs != NULL ){
      ctask->open_fs[ctask->next_free_node++] = found_fs;
      k_printf("Found!\n");
      k_printf("Fd will be %d\n", ctask->next_free_node - 1);
      return ctask->next_free_node - 1;
   }else{
      k_printf("Not Found!\n");  
   }
   return -1;
}

int sys_write(int fd, char *ubuf, int len){
   k_printf("Write called w/ %d bytes!\n", len);
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();
   
   char *kbuf = k_malloc(len, 0);
   vm_copy_from_pdir( (uint32_t)ubuf, ctask->task_info.task_pd, kbuf, len);

   ctask->open_fs[fd]->write( ctask->open_fs[fd], 0, len, kbuf);
   return 0;
}

int sys_read(int fd, char *ubuf, int len){
   k_printf("Readcalled!\n");
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();
   
   char *kbuf = k_malloc(len, 0);
   ctask->open_fs[fd]->read( ctask->open_fs[fd], 0, len, kbuf);

   k_printf("You read %d bytes ", len);
   for(int i = 0; i < len; i++)
      k_printf("%c", kbuf[i]);
   k_printf("\n");

   vm_copy_to_physical(kbuf, virt_to_phys(ALIGN_4K((uint32_t)ubuf), ctask->task_info.task_pd) + (uint32_t)ubuf & 0xfff, len);
   return 0; 
}



int read_fs(fs_node_t *node, int offset, int size, char *buffer){
   if( node->read != NULL )
      return node->read(node, offset, size, buffer);
   return -1;
}

int write_fs(fs_node_t *node, int offset, int size, char *buffer){
   if( node->write != NULL )
      return node->write(node, offset, size, buffer);
   return -1;
}

int open_fs(fs_node_t *node, int flags){
   if( node->open != NULL )
      return node->open(node, flags);
   return -1;
}

int close_fs(fs_node_t *node){
   if( node->close != NULL )
      return node->close(node);
   return -1;
}

dirent_t *readdir_fs(fs_node_t *node, int index){
   if( node->readdir != NULL && (node->flags & FS_DIRECTORY))
      return node->readdir(node, index);
   return NULL;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name){
   if( node->finddir != NULL && (node->flags & FS_DIRECTORY))
      return node->finddir(node, name);
   return NULL;
}
