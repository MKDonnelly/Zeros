#pragma once
#include <lib/types.h>
#include <fs/vfs/fsmanager.h>
#include <kernel/blkdev/blkdev.h>
#include <fs/vfs/vnode.h>

//See zsfs.c
extern fstype_t zsfs_fs;

//Type 0 == ZSFS filesystem
#define ZSFS_TYPE	0
#define ZSFS_MAGIC	"ZSFS"
#define ZSFS_MAGIC_LEN	4
#define ZSFS_NAME_MAX_LEN 24
//16 DSEs in a DS.  The last one is for the
//aux entry.
#define ZSFS_DSE_IN_DS	15

//The superblock.  Always at the beginning of
//the filesystem.  
typedef struct zsfs_sb{
   //The magic bytes: 'Z', 'S', 'F', 'S'
   char *magic[ZSFS_MAGIC_LEN];

   //The file system id (like the uuid)
   uint32_t fs_id;

   //The freeblock list block index and
   //length (in bits)
   uint32_t fbl_index;
   uint32_t fbl_len;

   //The root directory structure
   uint32_t rds_index;
}zsfs_sb_t;

//The Directory Structure Entry (DSE).
//Directories have 15 of these which can
//point to a subdirectory or file
typedef struct zsfs_dse{
   char type : 1;
   uint32_t blk_idx : 31;
   uint32_t entry_len;
   char entry_name[ZSFS_NAME_MAX_LEN]; 
}zsfs_dse_t;

//The last entry of a Directory Structure is
//the auxiliary entry, which can point to a linked
//directory if needed.
typedef struct zsfs_aux_entry{
   char unused[24];
   uint32_t parent_ds;
   //Next linked directory if not 0
   uint32_t linked_dir;
}zsfs_aux_entry_t;

//The Directory Structure (DS).  Used to
//hold the entries in a directory.
typedef struct zsfs_ds{
  zsfs_dse_t entries[ZSFS_DSE_IN_DS];
  zsfs_aux_entry_t aux_entry;
}zsfs_ds_t;


//A file on the drive
typedef struct zsfs_file{
   char data[508];
   uint32_t next_blk;
}zsfs_file_t;

//An inode describes where the DSE for a file/directory
//is stored on the drive.  The only exception is for the
//root directory, which only uses ds_blk_idx and points
//to the RDS instead of the RDE for this directory.
typedef struct zsfs_finode{
   uint32_t ds_blk_idx;
   uint32_t ds_entry_idx;
}zsfs_finode_t;

typedef struct zsfs_dinode{
   uint32_t blk_idx;
}zsfs_dinode_t;

//Registers this fs type.  Needed for every fs type implemented.
void zsfs_register();

//Create ZSFS on the given blkdev
void zsfs_create(blkdev_t *block);

//Check if we have a ZSFS file system given the
//first block from the drive
int zsfs_check(char *buf);

//Returns the filesystem id
uint32_t zsfs_get_id(char *block);

/////////////////////////////////////////////////////

//ZSFS implements lazy opening. A call to finddir_fs will return a 
//fs_node_t, but this will not result in an inode being allocated for
//the file. Instead, the node must call open in order for an inode
//to be allocated.
fs_node_t *zsfs_open(fs_node_t *root, char *name);
void zsfs_close(fs_node_t *fsnode); 
dirent_t *zsfs_readdir(fs_node_t *dir, int index); 
fs_node_t *zsfs_finddir(fs_node_t *dir, char *name); 
int zsfs_read(fs_node_t *file, int offset, int len, char *buffer);
int zsfs_write(fs_node_t *file, int offset, int len, char *buffer);
int zsfs_len(fs_node_t *file); 
int zsfs_delete(fs_node_t *zsfs_object);

fs_node_t *zsfs_get_root(fstype_t *zsfs);
