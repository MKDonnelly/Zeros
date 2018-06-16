#pragma once
#include <lib/types.h>
#include <kernel/blkdev/drive.h>

//Represents a block device on the system
//(Mainly hard drives)
typedef struct blkdev{
   //The drive that this block device resides on
   drive_t *parent;

   //Various pieces of information about the block device
   uint32_t min_lba;
   uint32_t max_lba;
   uint32_t block_size;

   //Common methods that all block devices should support 
   void (*write_lba)(struct blkdev *blkdev, char *buffer,
                     uint32_t lba, uint32_t size);
   void (*read_lba)(struct blkdev *blkdev, char *buffer,
                    uint32_t lba, uint32_t size);
}blkdev_t;
