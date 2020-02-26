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

   fs_node_t *found_fs = zsfs_open(root_fs, name);

   if( found_fs != NULL ){
      //Find a free spot to place the new node
      int i;
      for(i = 0; i < 20; i++){
         if( ctask->open_fs[i] == NULL ){
            ctask->open_fs[i] = found_fs;
            break;
         }
      }
      return i;
   }
   return -1;
}

int sys_close(int fd){
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();

   fs_node_t *to_close = ctask->open_fs[fd];
   int cval = to_close->close(to_close);
   ctask->open_fs[fd] = NULL;

   return cval;
}

//From can be 
//   SEEK_CUR = 0
//   SEEK_BEG = 1
//   SEEK_END = 2
#define SEEK_CUR	0
#define SEEK_BEG	1
#define SEEK_END	2
int sys_seek(int fd, int offset, int from){
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();
   fs_node_t *node = ctask->open_fs[fd];
   if( from == SEEK_CUR ){
      //Just return current offset
      return node->offset;
   }else if( from == SEEK_BEG ){
      //Move to position 0
      node->offset = 0;
      return 0;
   }else if( from == SEEK_END ){
      node->offset = node->len(node);
      return node->offset;
   }
}

int sys_write(int fd, char *ubuf, int len){
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();
   int w_bytes = ctask->open_fs[fd]->write( ctask->open_fs[fd], ctask->open_fs[fd]->offset, len, ubuf);
   ctask->open_fs[fd]->offset += w_bytes;
   return w_bytes;
}

int sys_read(int fd, char *ubuf, int len){
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();
   fs_node_t *f = ctask->open_fs[fd];
   int r_bytes = f->read(f, f->offset, len, ubuf);
   f->offset += r_bytes;
   return r_bytes;
}


int sys_getargs(char *ubuf, int len){
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();
   int max = strlen(ctask->args) > len ? len : strlen(ctask->args);
   memcpy(ubuf, ctask->args, max);

   //Return length of arguments
   return strlen(ctask->args);
}

int sys_createf(char *parent, char *child){
   fs_node_t *parent_dir = zsfs_open(root_fs, parent);

   if( parent_dir != NULL ){
      return zsfs_create_file(parent_dir, child);
   }else{
      return -1;
   } 

/*   ktask_t *ctask = current_scheduler->scheduler_current_ktask();

   fs_node_t *dir = ctask->open_fs[pdir_fd];

   return zsfs_create_file(dir, name);  */
}

/*int sys_created(int pdir_fd, char *name){
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();

   fs_node_t *dir = ctask->open_fs[pdir_fd];

   zsfs_create_dir(dir, name);

   //zsfs_create_dir(root_fs, name);
}*/

int sys_created(char *pdir, char *newdir){
   fs_node_t *parent = zsfs_open(root_fs, pdir);

   if( parent != NULL ){
      return zsfs_create_dir(parent, newdir);
   }else{
      return -1;
   }
}

int sys_exists(char *name){
   fs_node_t *f = zsfs_open(root_fs, name);
   if( f != NULL ){
      zsfs_close(f);
      return 1;
   }else{
      return 0;
   }
}

int is_directory(char *name){
   fs_node_t *f = zsfs_open(root_fs, name);
   if( f != NULL ){
      if( f->type == FS_DIRECTORY ){
         zsfs_close(f);
         return 1;
      }else{
         zsfs_close(f);
         return 0;
      }
   }else{
      return -1;
   }
}

int sys_getpwd(char *buf, int maxlen){
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();
   int max = strlen(ctask->pwd)+1 > maxlen ? maxlen : strlen(ctask->pwd)+1;
   memcpy(buf, ctask->pwd, max);
   return 0;
}

int sys_chdir(char *dir){
   if( sys_exists(dir) && is_directory(dir) ){
      ktask_t *ctask = current_scheduler->scheduler_current_ktask();
      strcpy(ctask->pwd, dir);
      return 0;
   }else{
      return -1;
   }
}

int sys_readdir(int fd, char *buf, int len, int idx){
   ktask_t *ctask = current_scheduler->scheduler_current_ktask();
   fs_node_t *readfrom = ctask->open_fs[fd];
   dirent_t *d = zsfs_readdir(readfrom, idx);
   if( d != NULL ){
      int maxlen = strlen(d->name) > len ? len : strlen(d->name);
      memcpy(buf, d->name, maxlen);
      k_free(d);
      return 0;
   }else{
      return -1;
   } 
}

int sys_delete(char *name){
   fs_node_t *f = zsfs_open(root_fs, name);
   int v = zsfs_delete(f);
   f->close(f);
   return 0;
}
