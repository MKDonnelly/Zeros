#include <fs/vfs/blkfs.h>
#include <fs/vfs/fs.h>
#include <kernel/blkdev/blkdev.h>
#include <kernel/mm/heap.h>

//Handles all of the known types of file systems
//as well as managing blkfs_t's
#define MAX_BLKFS	10
static blkfs_t *blkfses[MAX_BLKFS];
static int blkfs_count = 0;

#define MAX_FSES	5
static fstype_t *known_fses[MAX_FSES];
static int fs_count = 0;

//This only create a new blkfs_t if a filesystem recognizes it.
void blkfs_register_blkdev(blkdev_t *blockdev){
   //Read the first block of the block device. Iterate
   //over each known filesystem and find one that matches.
   char *temp_buf = k_malloc(blockdev->block_size, 0);
   blockdev->read_lba( blockdev, temp_buf, 0, 1 );

   for(int i = 0; i < fs_count; i++){
      if( known_fses[i]->check_type(temp_buf) != 0 ){
         blkfs_t *new_blkfs = k_malloc(sizeof(blkfs_t), 0);
         new_blkfs->fstype = known_fses[i]->type_id;
         new_blkfs->parent = blockdev;
         new_blkfs->filesystem_type = known_fses[i];
         new_blkfs->fs_id = known_fses[i]->get_id(temp_buf);
         blkfses[blkfs_count] = new_blkfs;
         blkfs_count++;
         return;
      }
   }
}

void blkfs_register_fstype(fstype_t *fstype){
   known_fses[fs_count] = fstype;
   fs_count++;
}

blkfs_t *blkfs_find_id(uint32_t id){
   for(int i = 0; i < blkfs_count; i++){
      if(blkfses[i]->fs_id == id )
         return  blkfses[i];
   }
}
