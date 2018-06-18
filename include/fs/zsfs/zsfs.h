#pragma once
#include <lib/types.h>
#include <fs/vfs/fsmanager.h>
#include <kernel/blkdev/blkdev.h>

extern fstype_t zsfs_fs;

#define ZSFS_MAGIC	"ZSFS"
#define ZSFS_LEN	4

//The superblock, describing this filesystem.
//It is 512 bytes in length, to fit into one block
typedef struct zsfs_sb{
   //The magic bites: 'Z', 'S', 'F', 'S', '\0'
   char *magic[ZSFS_LEN];
   //The file system id (like the uuid)
   uint32_t fs_id;

   //The freeblock list
   int freeblock_block;

   //The root directory entry
   int fsentry_block;
}zsfs_sb_t;

void zsfs_init();
void zsfs_create(blkdev_t *block);
uint32_t zsfs_get_id(char *block);
