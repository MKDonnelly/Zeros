#pragma once

#include <kernel/blkdev/blkdev.h>
#include <fs/vfs/vnode.h>

//Represents common functions that any file system
//must implement. Individual filesystem fill out this structure
//and pass it to fsmanager_register_fstype.
typedef struct fstype{
   //Each filesystem has a corresponding type
   int fs_type;

   //A global id for this filesystem
   int fs_id;

   //The block device that this filesystem lies on.
   blkdev_t *parent;

   //Used when discovering the file system. check_type
   //accepts a buffer that contains the first block in
   //the drive. It checks for fs-specific magic numbers
   //and returns non-zero if the filesystem belongs to it,
   //zero else.
   int (*check_type)(char *start_block);

   //Returns a globally unique id corresponding to the filesystem.
   //with id equal to the return value, we should be able to retrieve
   //this structure using
   //    fstype_t *this_struct = fsmanager_find_id(id);
   //TODO revise this: remove the fs_id member and let this directly
   //                  access the blkdev
   uint32_t (*get_id)(char *buf);

   //Linked list of open inodes. When we are passed an arbitrary
   //fs_node_t, we get the inode field and search for the corresponding
   //entries in this list. When a file is closed, the inode is deleted.
   //THE INODE FORMAT IS FILESYSTEM-SPECIFIC, HENCE A void*
   void *open_inodes;

   //The superblock for the filesystem. Each filesystem will necessarily
   //have a different format, so this will be a void*. This can store
   //any other miscelaneous information, if needed.
   void *superblock;

   //Operations that are performed on the root directory.
   fs_node_t root_dir;
}fstype_t;

//Check to see if a given block device contains a known file system
void fsmanager_check(blkdev_t *blkdev);

//Register a new filesystem with the fsmanager
void fsmanager_register_fstype(fstype_t *fstype);

void fsmanager_add_active_fs(fstype_t *new_fs);

//Search for an active filesystem given its unique 32-bit ID
fstype_t *fsmanager_find_id(uint32_t id);
