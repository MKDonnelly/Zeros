#pragma once
#include <lib/types.h>
#include <fs/vfs/fsmanager.h>
#include <kernel/blkdev/blkdev.h>
#include <fs/vfs/vnode.h>

extern fstype_t zsfs_fs;

#define ZSFS_TYPE	0
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
int zsfs_check(char *buf);

uint32_t zsfs_get_id(char *block);
int zsfs_read(fs_node_t *file, int offset, int len, char *buffer);
int zsfs_write(fs_node_t *file, int offset, int len, char *buffer);

//ZSFS implements lazy opening. A call to finddir_fs will return a 
//fs_node_t, but this will not result in an inode being allocated for
//the file. Instead. the node must call open in order for an inode
//to be allocated.
int zsfs_open(fs_node_t *fsnode, int flags);
int zsfs_close(fs_node_t *fsnode);
dirent_t *zsfs_readdir(fs_node_t *dir, int index);
fs_node_t *zsfs_finddir(fs_node_t *dir, char *name);
