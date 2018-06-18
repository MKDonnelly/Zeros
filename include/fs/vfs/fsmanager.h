#pragma once

#include <fs/vfs/vnode.h> 
#include <kernel/blkdev/blkdev.h>

struct fs_obj;
//Represents common functions that any file system
//must implement
typedef struct fstype{
   int fs_type;
   int fs_id;
   blkdev_t *parent;

   int (*check_type)(char *start_block);
   char *(*get_superblock)(struct fstype *self);
   uint32_t (*get_id)(char *buf);
   struct fs_obj *open(struct fstype *self, int index struct fs_obj *);
}fstype_t;

//Check to see if a given block device contains a known file system
void fsmanager_check(blkdev_t *blkdev);

//Register a new filesystem with the fsmanager
void fsmanager_register_fstype(fstype_t *fstype);

//Search for an active filesystem given its unique 32-bit ID
fstype_t *fsmanager_find_id(uint32_t id);
