#include <fs/initrd/initrd.h>

#include <kernel/mm/heap.h>
#include <kernel/blkdev/blkdev.h>
#include <lib/types.h>
#include <lib/string.h>
#include <fs/vfs/fsmanager.h>
#include <fs/vfs/vnode.h>
#include <lib/genericll.h>

#define INITRD_MAGIC 0x12345678

#define FS_INITRD 1

int initrd_sizeof(initrd_sb_t *initrd, char *name){
   for(int i = 0; i < initrd->entries_len; i++){
      initrd_entry_t *e = (initrd_entry_t*)((size_t)initrd + (size_t)initrd->entries_offset + i * sizeof(initrd_entry_t));
      if( strcmp(e->name, name) == 0 ){
         return e->entry_len;
      }
   }
}

int initrd_read(initrd_sb_t *initrd, char *name, int len, char *buffer){

   int ibuf = 0;
   //Search through entries in sb to find the file
   for(int i = 0; i < initrd->entries_len; i++){
      initrd_entry_t *e = (initrd_entry_t*)((size_t)initrd + (size_t)initrd->entries_offset + i * sizeof(initrd_entry_t));
      if( strcmp(e->name, name) == 0 ){
         ibuf = 0;
         char *file = (char*)((size_t)initrd + (size_t)e->entry_offset);
         while((ibuf < len) && (ibuf < e->entry_len)){
            buffer[ibuf] = file[ibuf];
            ibuf++;
         }
      }
      e++;
   }

   return ibuf;
}
