#include <fs/vfs/fsmanager.h>
#include <kernel/blkdev/blkdev.h>
#include <kernel/mm/heap.h>
#include <lib/string.h>

//Manages registering/unregistering file system types and 
//matching block devices discovered to the appropriate 
//filesystem driver.

//These are intended to be templates; when an actual block
//device containing the file system comes in, the handler will
//take a copy and fill in several fields (namely, the fs_id and parent)
#define MAX_FSES	5
static fstype_t *template_fses[MAX_FSES];
static int template_fses_count = 0;

//The actual active file systems. Everything here will be a copy
//from template_fses, with some extra fields filled in
static fstype_t *active_fses[MAX_FSES];
static int active_fses_count = 0;

//Checks to see if the given blockdev has a valid filesystem on it
//by iterating through each template filesystem and calling check_type.
void fsmanager_check(blkdev_t *blockdev){
   //Read the first block of the block device. Iterate
   //over each template filesystem and find one that matches.
   char *temp_buf = k_malloc(blockdev->block_size, 0);
   blockdev->read_lba( blockdev, 0, 1, temp_buf );

   for(int i = 0; i < template_fses_count; i++){
      //TODO let the filesystems themselves do the I/O and return
      //     a fstype_t.
      if( template_fses[i]->check_type(temp_buf) != 0 ){
         fstype_t *new_active_fs = k_malloc(sizeof(fstype_t), 0);
         memcpy(new_active_fs, template_fses[i], sizeof(fstype_t));
         new_active_fs->fs_id = template_fses[i]->get_id(temp_buf);
         new_active_fs->parent = blockdev;
         
         active_fses[active_fses_count] = new_active_fs;
         active_fses_count++;
         return;
      }
   }
}

//Register a new filesystem type. This must be called by
//each filesystem that needs to be used. Files system not
//registered with this may as well not exist.
void fsmanager_register_fstype(fstype_t *fstype){
   template_fses[template_fses_count] = fstype;
   template_fses_count++;
}

//Directly add an instance of a file system into active_fses
//This is primarily here to let the initrd bypass the drive/blkdev
//interface and directly add a file system.
void fsmanager_add_active_fs(fstype_t *new_fs){
   active_fses[active_fses_count] = new_fs;
   active_fses_count++;
}

//Find an active file system given its 32-bit unique ID
fstype_t *fsmanager_find_id(uint32_t id){
   for(int i = 0; i < active_fses_count; i++){
      if(active_fses[i]->fs_id == id )
         return active_fses[i];
   }
   return NULL;
}
