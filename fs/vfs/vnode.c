#include <fs/vfs/vnode.h>
#include <kernel/sched/sched.h>
#include <arch/current_arch>
#include <kernel/task.h>
#include <lib/print.h>
#include <kernel/mm/heap.h>
#include <arch/current_arch>


extern fs_node_t *root_fs;

//System calls
int sys_open(char *name){
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();

   char buf[30];
   vm_copy_str((uint32_t)name, ctask->task_info.task_pd, buf, 30);

   fs_node_t *found_fs = root_fs->finddir(root_fs, buf);
   if( found_fs != NULL ){
      ctask->open_fs[ctask->next_free_node++] = found_fs;
      k_printf("Found! Fd will be %d\n", ctask->next_free_node - 1);
      return ctask->next_free_node - 1;
   }else{
      k_printf("Not Found!\n");  
   }
   return -1;
}

int sys_write(int fd, char *ubuf, int len){
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();
   
   char *kbuf = k_malloc(len, 0);
   vm_copy_from_pdir( (uint32_t)ubuf, ctask->task_info.task_pd, kbuf, len);

   ctask->open_fs[fd]->write( ctask->open_fs[fd], 0, len, kbuf);

   //TODO return number of bytes writen
   return 0;
}

int sys_read(int fd, char *ubuf, int len){
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();
   
   char *kbuf = k_malloc(len, 0);
   ctask->open_fs[fd]->read( ctask->open_fs[fd], 0, 1, kbuf);

   k_printf("You read %d bytes ", len);
   for(int i = 0; i < len; i++)
      k_printf("%c", kbuf[i]);
   k_printf("\n");

   vm_copy_to_physical(kbuf, virt_to_phys(ALIGN_4K((uint32_t)ubuf), ctask->task_info.task_pd) + ((uint32_t)ubuf & 0xfff), len);

   //todo return number of bytes read
   return 0; 
}

