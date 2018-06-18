#include <fs/vfs/fsmanager.h>
#include <kernel/blkdev/blkdev.h>
#include <kernel/mm/heap.h>

//Manages registering/unregistering file system types and 
//matching block devices discovered to the appropriate 
//filesystem driver.

//Handles all of the known types of file systems.
//These are intended to be templates; when an actual block
//device containing the file system comes in, the handler will
//take a copy and fill in several fields (namely, the fs_id and parent)
#define MAX_FSES	5
static fstype_t *known_fses[MAX_FSES];
static int known_fses_count = 0;

//The actual active file systems. Everything here will be a copy
//from known_fses, with some extra fields filled in
static fstype_t *active_fses[MAX_FSES];
static int active_fses_count = 0;

//Checks to see if the given blockdev has a valid filesystem on it
//by iterating through each known filesystem and calling check_type.
void fsmanager_check(blkdev_t *blockdev){
   //Read the first block of the block device. Iterate
   //over each known filesystem and find one that matches.
   char *temp_buf = k_malloc(blockdev->block_size, 0);
   blockdev->read_lba( blockdev, temp_buf, 0, 1 );

   for(int i = 0; i < known_fses_count; i++){
      if( known_fses[i]->check_type(temp_buf) != 0 ){
         fstype_t *new_active_fs = k_malloc(sizeof(fstype_t), 0);
         memcpy(new_active_fs, known_fses[i], sizeof(fstype_t));
         new_active_fs->fs_id = known_fses[i]->get_id(temp_buf);
         new_active_fs->get_superblock = known_fses[i]->get_superblock;
         new_active_fs->parent = blockdev;
/*
         new_active_fs->fs_type = known_fses[i]->fs_type;
         new_active_fs->fs_id = known_fses[i]->get_id(temp_buf);
         new_active_fs->parent = blockdev;
         new_active_fs->check_type = known*/

         active_fses[active_fses_count] = new_active_fs;
         active_fses_count++;
         return;
      }
   }
}

//Register a new filesystem type. 
void fsmanager_register_fstype(fstype_t *fstype){
   known_fses[known_fses_count] = fstype;
   known_fses_count++;
}

//Find an active file system given its 32-bit unique ID
fstype_t *fsmanager_find_id(uint32_t id){
   for(int i = 0; i < active_fses_count; i++){
      if(active_fses[i]->fs_id == id )
         return active_fses[i];
   }
   return NULL;
}
