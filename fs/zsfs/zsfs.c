#include <fs/zsfs/zsfs.h>
#include <kernel/mm/heap.h>
#include <lib/string.h>
#include <lib/print.h>

#define ROOT_INODE	0

fstype_t zsfs_fs = {
   .fs_type = ZSFS_TYPE,
   .fs_id = 0, //we don't know until we instantiate the fs
   .parent = NULL, //again, we can't tell
   .check_type = zsfs_check, //used to check the fs type
   .get_id = zsfs_get_id, //get unique id. this goes in fs_id above
   .open_inodes = NULL, //linked list of open indoes
   .root_dir = {
      .name = "/",
      .flags = 0,
      .fs = NULL,
      .inode = 0,  //Inodes are assigned sequentially

      .read = zsfs_read,
      .write = zsfs_write,
      .open = zsfs_open,
      .close = zsfs_close,
      .readdir = zsfs_readdir,
      .finddir = zsfs_finddir,
   },
};

//Register the filesystem with the fsmanager
void zsfs_init(){
   fsmanager_register_fstype(&zsfs_fs);
}

int zsfs_check(char *buf){
   if( strncmp( buf, ZSFS_MAGIC, ZSFS_LEN) == 0 ){
      k_printf("[zsfs] Found filesystem on blkdev\n");
      return 1;
   }
   return 0;
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


int zsfs_read(fs_node_t *file, int offset, int len, char *buffer){

}

int zsfs_write(fs_node_t *file, int offset, int len, char *buffer){

}

int zsfs_open(fs_node_t *fsnode, int flags){

}

int zsfs_close(fs_node_t *fsnode){

}

dirent_t *zsfs_readdir(fs_node_t *dir, int index){

}

fs_node_t *zsfs_finddir(fs_node_t *dir, char *name){

}

/*char *zsfs_get_sb(fstype_t *fstype){
   char *sb = k_malloc(sizeof(zsfs_sb_t), 0);
   char *buf = k_malloc(fstype->parent->block_size, 0);

   fstype->parent->read_lba( fstype->parent, buf, 0, 1 );
   memcpy( sb, buf, sizeof(zsfs_sb_t));
    
   k_free(buf);
   return sb;
}*/

