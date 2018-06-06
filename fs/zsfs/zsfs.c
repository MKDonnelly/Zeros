#include <fs/zsfs/zsfs.h>
#include <fs/mbr.h>
#include <kernel/mm/heap.h>
#include <drivers/drive.h>
#include <lib/string.h>


//Offset of various blocks into the partition
#define SUPERBLOCK_SECTOR	0
#define FREELIST_SECTOR		1
#define FSENTRIES_SECTOR	2

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

   //By default, put the freelist in block 1
   superblock.freelist_block = start_lba + FREELIST_SECTOR;

   //By default, put the file allocation block in block 2
   superblock.fsentries_block = start_lba + FSENTRIES_SECTOR;
   superblock.fsentries_count = 0;

   //Zero out the freelist and fsentries block
   char *zeros = k_malloc( 512, 0 );
   memset( zeros, 512, 0 );

   drive->drive_write_lba( drive, zeros, superblock.freelist_block, 1 );
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
