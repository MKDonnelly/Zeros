#pragma once
#include <lib/types.h>
#include <kernel/blkdev/drive.h>

//Represents a block device on the system
//(Mainly hard drives)
typedef struct blkdev{
   //The drive that this block device resides on
   drive_t *parent;

   //Various pieces of information about the block device
   size_t min_lba;
   size_t max_lba;
   size_t block_size;

   //Common methods that all block devices should support 
   //TODO standardize the argument order (i.e. buffer at end)
   //     also make naming consistent (i.e. size should be len)
   void (*write_lba)(struct blkdev *blkdev, char *buffer,
                     size_t lba, size_t len);
   void (*read_lba)(struct blkdev *blkdev, char *buffer,
                    size_t lba, size_t len);
}blkdev_t;

void blkdev_register(blkdev_t *blkdev);
blkdev_t *blkdev_find(int id);
