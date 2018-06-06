#pragma once
#include <lib/types.h>
#include <drivers/drive.h>

//The superblock, describing this filesystem.
//It is 512 bytes in length, to fit into one block
typedef struct superblock{
   uint32_t block_size;
   uint32_t total_blocks;
   //The LBA number of a bitmap keeping track of which
   //blocks are free. The last 32 bits are reserved and 
   //are set to 0; when the first block fills up, this will
   //hold the address of the next block.
   uint32_t freelist_block;

   int fsentries_count;
   //Holds an array of fsentry_t's.
   uint32_t fsentries_block;
}superblock_t;

//A file system entry.
//Note: EXACTLY 6 of these will fit into a 512 byte block,
//with 4 bytes left over for the next block pointer.
typedef struct fsentry{
   char name[16];
   uint32_t total_occupied_blocks;
   uint32_t blocks[16];
}fsentry_t;

void zsfs_create(drive_t *drive, int partition);
superblock_t *zsfs_get_superblock(drive_t *drive, int partition_number);
