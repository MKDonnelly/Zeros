#include <kernel/blkdev/blkdev.h>
#include <fs/vfs/fsmanager.h>

//A block device is intended to represent a single partition
//on a drive. To do that, we need to limit the range of the 
//drive that the blockdev can access. These do that. Write/reads
//are intended to start from the start of the blkdev.
void blkdev_write_lba(blkdev_t *dev, char *buf, 
                      uint32_t lba, uint32_t len){
   int drive_len = dev->max_lba - dev->min_lba;
   if( (lba + len) > drive_len || (lba + len) < 0 ){
      //Asked to write outside of drive.
   }else{
       dev->parent->write_lba(dev->parent, buf, dev->min_lba + lba, len);
   }
}

void blkdev_read_lba(blkdev_t *dev, char *buf,
                     uint32_t lba, uint32_t len){
   int drive_len = dev->max_lba - dev->min_lba;
   if( (lba + len) > drive_len || (lba + len) < 0 ){
      //Asked to write outside of drive.
   }else{
       dev->parent->read_lba(dev->parent, buf, dev->min_lba + lba, len);
   }
}

//Handles registering and searching for block devices
#define MAX_BLKDEVS	10
static blkdev_t *blkdevs[MAX_BLKDEVS];
static int blkdevs_count = 0;

void blkdev_register(blkdev_t *blkdev){
   blkdev->write_lba = blkdev_write_lba;
   blkdev->read_lba = blkdev_read_lba;
   blkdevs[blkdevs_count] = blkdev;
   blkdevs_count++;
   
   //Now see if there is any file system to register
   //on this block device
   fsmanager_check(blkdev);
}

blkdev_t *blkdev_find(int id){
   return blkdevs[id];
}
