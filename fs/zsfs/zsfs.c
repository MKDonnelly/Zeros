#include <fs/zsfs/zsfs.h>
#include <kernel/mm/heap.h>
#include <lib/string.h>
#include <lib/print.h>
#include <fs/vfs/vfs.h>

void zsfs_init(){
   fsmanager_register_fstype(&zsfs_fs);
}

int zsfs_check(char *buf){
   if( strncmp( buf, ZSFS_MAGIC, ZSFS_LEN) == 0 ){
      k_printf("[zsfs] Found filesystem on blkdev\n");
      return 1;
   }else{
      return 0;
   }
}

void zsfs_create(blkdev_t *block){
   zsfs_sb_t new_sb;
   memcpy(new_sb.magic, ZSFS_MAGIC, ZSFS_LEN);
   new_sb.fs_id = 0x1234ABCD;

   //start freeblock on block 1
   new_sb.freeblock_block = 1;

   //Find length of freelist block needed. Add 1 to take care of
   //any partial blocks needed. Add another 1 to go beyond the freeblock
   //so that the fsentry block does not collide.
   new_sb.fsentry_block = ((block->max_lba - block->min_lba) / 
                           block->block_size) + 2;

   //Zero out the drive up to the end of the fsentry_block
   char *zeros = k_malloc(block->block_size, 0);
   memset(zeros, 0, block->block_size);

   for(int i = 0; i < new_sb.fsentry_block; i++){
      block->write_lba( block, zeros, i, 1 );
   }

   block->write_lba( block, (char*)&new_sb, 0, sizeof(zsfs_sb_t));
}

uint32_t zsfs_get_id(char *buf){
   zsfs_sb_t *sb = (zsfs_sb_t*)buf;
   return sb->fs_id;
}

char *zsfs_get_sb(fstype_t *fstype){
   char *sb = k_malloc(sizeof(zsfs_sb_t), 0);
   char *buf = k_malloc(fstype->parent->block_size, 0);

   fstype->parent->read_lba( fstype->parent, buf, 0, 1 );
   memcpy( sb, buf, sizeof(zsfs_sb_t));
    
   k_free(buf);
   return sb;
}

fs_obj_t *zsfs_open(fstype_t *self, int index, fs_obj_t *dir){
   if( dir == NULL ){
      
   }
}

fstype_t zsfs_fs = {
   .fs_type = 0,
   .check_type = zsfs_check,
   .get_superblock = zsfs_get_sb,
   .get_id = zsfs_get_id,
   .open = zsfs_open,
};
