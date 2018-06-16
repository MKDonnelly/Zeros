#include <fs/zsfs/zsfs.h>
#include <kernel/mm/heap.h>
#include <lib/string.h>
#include <fs/vfs/fs.h> 


void zsfs_init(){
   blkfs_register_fstype(&zsfs_fs);
}

int zsfs_check(char *buf){
   k_printf("ZSFS checking...\n");
   if( strncmp( buf, ZSFS_MAGIC, ZSFS_LEN) == 0 ){
      k_printf("WE FOUND A MATCH!!!!!!!!!!\n");
      return 1;
   }else{
      return 0;
   }
}

void zsfs_create(blkdev_t *block){
   zsfs_sb_t new_sb;
   memcpy(new_sb.magic, ZSFS_MAGIC, ZSFS_LEN);
   new_sb.fs_id = 0x1234ABCD;
   block->write_lba( block, &new_sb, 0, sizeof(zsfs_sb_t));
}

uint32_t zsfs_get_id(char *buf){
   zsfs_sb_t *sb = (zsfs_sb_t*)buf;
   return sb->fs_id;
}

fstype_t zsfs_fs = {
   .type_id = 0,
   .check_type = zsfs_check,
   .get_superblock = NULL,
   .get_id = zsfs_get_id,
   .get_node = NULL,
};

/*
//Offset of various blocks into the partition
#define SUPERBLOCK_BLOCK	0
#define FREELIST_BLOCK		1
#define FSENTRIES_BLOCK		2

//Create a ZSFS file system on the given partition
void zsfs_create(drive_t *drive, int partition_number){

   //Get the start lba of the partition
   uint32_t start_lba = drive->drive_part_lba_start(drive, 
                                                    partition_number);

   //Construct a temporary buffer and write it to the
   //drive. 
   superblock_t superblock;
   superblock.block_size = 512;
   superblock.total_blocks = drive->drive_part_size(drive, partition_number);

   //By default, put the freelist in block 1. Calculate how far
   //it needs to go.
   superblock.freelist_start_block = start_lba + FREELIST_BLOCK;
   superblock.freelist_len = superblock.freelist_start_block + 
                             (drive->drive_part_size(drive, partition_number) / 512) + 1;

   //By default, put the file allocation block after the freelist block
   superblock.fsentries_block = superblock.freelist_len + 
                                         superblock.freelist_start_block;
   superblock.fsentries_count = 0;

   //Zero out the freelist and fsentries block
   char *zeros = k_malloc( 512, 0 );
   memset( zeros, 512, 0 );

   for(int i = 0; i < superblock.freelist_len; i++){
      drive->drive_write_lba( drive, zeros, 
                              superblock.freelist_start_block + i, 1 );
   }
   drive->drive_write_lba( drive, zeros, superblock.fsentries_block, 1);

   //write superblock
   memcpy( zeros, &superblock, sizeof(superblock_t));
   drive->drive_write_lba( drive, zeros, start_lba, 1 );
}

superblock_t *zsfs_get_superblock(drive_t *drive, int partition_number){
   superblock_t *superblock = k_malloc(sizeof(superblock_t), 0);

   //get lba of the superblock
   uint32_t start_lba = drive->drive_part_lba_start( drive, 
                                                     partition_number);

   char *buf = k_malloc(512, 0);
   drive->drive_read_lba( drive, buf, start_lba, 1 );

   memcpy( superblock, buf, sizeof(superblock_t));
   k_free(buf);
   return superblock;
}


void zsfs_create_file(drive_t *drive, int part_num, superblock_t *sb, char *name){
   fsentry_t new_entry;
   strcpy(new_entry.name, name);
   new_entry.byte_len = 0;
   
   //Get file system entry block
   fsentry_t *fseblock = k_malloc( sb->block_size, 0 );
   drive->drive_read_lba( drive, fseblock, FSENTRIES_BLOCK, 1 );

   int index = 0;
   while( fseblock[index].name[0] != 0 && fseblock[index].byte_len != 0 ){
      index++;
   }   

   memcpy(&fseblock[index], &new_entry, sizeof(fsentry_t));
   drive->drive_write_lba( drive, fseblock, FSENTRIES_BLOCK, 1 );
   k_free( fseblock);
}

fsentry_t *zfs_read_file(drive_t *drive, int part_num, superblock_t *sb){
   fsentry_t *ret = k_malloc(sizeof(fsentry_t), 0);
   
   fsentry_t *fsentry_block = k_malloc( sb->block_size, 0 );
   drive->drive_read_lba( drive, fsentry_block, FSENTRIES_BLOCK, 1);
   memcpy(ret, fsentry_block, sizeof(fsentry_t));
   k_free(fsentry_block);
   return ret;
}*/
