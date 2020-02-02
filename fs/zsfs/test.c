#include <fs/zsfs/zsfs.h>
#include <kernel/mm/heap.h>
#include <lib/string.h>
#include <lib/print.h>

#define ROOT_INODE	0

//We commonly need to write a zeroed out block.
//It is easier to do this rather than dynamically
//allocate, zero, write, and unallocate memory.
char zeros[512];

//This is like a class.  We create a copy of this for every
//ZSFS filesystem being used.
fstype_t zsfs_fs = {
   .fs_type = ZSFS_TYPE,
   .fs_id = 0, //we don't know until we instantiate the fs
   .parent = NULL, //again, we can't tell
   .check_type = zsfs_check, //used to check the fs type
   .get_id = zsfs_get_id, //get unique id. this goes in fs_id above
   .open_inodes = NULL, //array of open inodes
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
void zsfs_register(){
   fsmanager_register_fstype(&zsfs_fs);
}

//Initializes information about the filesystem 
int zsfs_init(fstype_t *zsfs_fs){
   //For now we can just have this be an array of
   //10 pointers to ZSFS inodes.  In the future,
   //I could expand this to be a linked list or
   //something so that it can dynamically handle
   //as many open files as needed.
   zsfs_fs->open_inodes = k_malloc(sizeof(zsfs_dse_t*) * 10, 0);
   memset(zsfs_fs->open_inodes, sizeof(zsfs_dse_t*)*10, 0);
   
}

int zsfs_check(char *buf){
   if( strncmp( buf, ZSFS_MAGIC, ZSFS_MAGIC_LEN) == 0 ){
      k_printf("[zsfs] Found filesystem on blkdev\n");
      return 1;
   }
   return 0;
}

void zsfs_create(blkdev_t *block){
   k_printf("Creating new zsfs filesystem\n");
   //The new superblock
   zsfs_sb_t new_sb;
   //Set magic field
   memcpy(new_sb.magic, ZSFS_MAGIC, ZSFS_MAGIC_LEN);

   //In the future, have this be a random value
   new_sb.fs_id = 0x1234ABCD;

   //Start the FBL at block index 1 (512 bytes into the drive)
   new_sb.fbl_index = 1;

   //Calculate how big the FBL needs to be.  Every 1 bit in the FBL
   //records one block in the Available Space segment.

   //Add a 1 to take care of any fractional block requirements
   new_sb.fbl_len = (((block->max_lba - block->min_lba) / block->block_size) + 1) * 512;

   //The root directory entry comes right after the FBL
   new_sb.rde_index = new_sb.fbl_index + (new_sb.fbl_len / 512);

   //Initialize the FBL with all zeros, and then set the blocks that 
   //the superblock, FBL, and RDE use as allocated.
   memset(zeros, block->block_size, 0);
   for(int i = new_sb.fbl_index; i < new_sb.fbl_index + new_sb.fbl_len / 512; i++){
      //The "1" is the number of blocks to write, which is 1 in this case.
      block->write_lba( block, i, 1, zeros );
   }

   //Set the first few bits for the superblock, FBL, and RDE
   for(int i = 0; i <= new_sb.rde_index; i++){
      bit_set(zeros, i);
   }
   block->write_lba(block, new_sb.fbl_index, 1, zeros);


   //Also, zero out the RDE since it starts empty
   memset(zeros, block->block_size, 0);
   block->write_lba(block, new_sb.rde_index, 1, zeros);

   //Write the new superblock.
   block->write_lba( block, 0, sizeof(zsfs_sb_t), (char*)&new_sb);
}

uint32_t zsfs_get_id(char *buf){
   zsfs_sb_t *sb = (zsfs_sb_t*)buf;
   return sb->fs_id;
}

///////////////////////////////////////////////////////////

int mkdir(char *parent, char *name, fstype_t *root);
int rmdir(char *name, fstype_t *root);

fs_node_t *open(char *path, fstype_t *zsfs);
int touch(char *parent, char *name, fstype_t *zsfs);
int read(fs_type_t *file, char *buf, size_t len, size_t offset);
int write(fs_type_t *file, char *buf, size_t len, size_t offset);

///////////////////////////////////////////////////////////








int zsfs_read(fs_node_t *file, int offset, int len, char *buffer){
   return 0;
}

int zsfs_write(fs_node_t *file, int offset, int len, char *buffer){
   return 0;
}

int zsfs_open(fs_node_t *fsnode, int flags){
   return 0;
}

int zsfs_close(fs_node_t *fsnode){
   return 0;
}

dirent_t *zsfs_readdir(fs_node_t *dir, int index){
   return NULL;
}

fs_node_t *zsfs_finddir(fs_node_t *dir, char *name){
   return NULL;
}
