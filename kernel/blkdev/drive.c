#include <kernel/blkdev/drive.h>
#include <kernel/mm/heap.h>
#include <kernel/blkdev/blkdev.h>


//A registering system for devices on the system
#define MAX_DRIVES	10
static drive_t *drives[MAX_DRIVES];
static int current_drive = 0;

void drive_register(drive_t *drive){
   drives[current_drive] = drive;
   drive->id = current_drive;
   current_drive++;

   //Now enumerate each blkdev within the drive
   //and register each.
   for(int i = 1; i <= drive->partitions.count; i++){
      blkdev_t *new_blkdev = k_malloc(sizeof(blkdev_t), 0);
      new_blkdev->parent = drive;
      new_blkdev->min_lba = drive->partitions.part_start(drive, i); 
      new_blkdev->max_lba = new_blkdev->min_lba + 
                                   drive->partitions.part_len(drive, i);
      new_blkdev->block_size = drive->blksize;
      
      k_printf("Blkdev %d: %d, %d, %d\n", i, 
          new_blkdev->min_lba, new_blkdev->max_lba, new_blkdev->block_size);
      blkdev_register(new_blkdev);
   }
}

drive_t *drive_search(int id){
   for(int i = 0; i < current_drive; i++){
      if( drives[i]->id == id )
         return &drives[i];
   }
}

