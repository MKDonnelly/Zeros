#pragma once
#include <fs/vfs/fs.h>
#include <kernel/blkdev/blkdev.h>

//Represents a file system on a block device
typedef struct blkfs{
   blkdev_t *parent;

   int fstype;
   uint32_t fs_id;
   fstype_t *filesystem_type;   
}blkfs_t;

