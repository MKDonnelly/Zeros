#pragma once
#include <lib/types.h>
#include <fs/vfs/fs.h>
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
   
}zsfs_sb_t;

void zsfs_create(blkdev_t *block);
uint32_t zsfs_get_id(char *block);
/*
//A file system entry.
//Note: EXACTLY 6 of these will fit into a 512 byte block,
//with 4 bytes left over for the next block pointer.
typedef struct fsentry{
   char name[16];
   uint32_t byte_len;
   uint32_t blocks[16];
}fsentry_t;

void zsfs_create(drive_t *drive, int partition);
superblock_t *zsfs_get_superblock(drive_t *drive, int partition_number);
void zsfs_create_file( drive_t *drive, int partition_number, superblock_t *superblock, char *name);*/
