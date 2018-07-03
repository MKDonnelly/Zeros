#pragma once
#include <lib/types.h>

struct drive;
typedef struct part{
   //Information on partitions:
   //the total number of partitions and
   //a void pointer to data that a specific
   //partition handler can use.
   int count;
   void *data;

   //Used for finding out information about
   //parititions.
   size_t (*part_start)(struct drive *drive, 
                          int part_num);
   size_t (*part_len)(struct drive *drive,
                        int part_num);
}part_t;

//Represents a storage drive attached

typedef struct drive{
   int id, type, blksize, maxlba;
   
   part_t partitions;
   
   //Common methods that all drives should support 
   void (*write_lba)(struct drive *drive, char *buffer,
                     size_t lba, size_t len);
   void (*read_lba)(struct drive *drive, char *buffer,
                    size_t lba, size_t len);
   void (*reset)();

   //This will store data specific to a drive.
   //i.e. the ATA pio mode drive may have this point
   //to a ata_pio_drive_t struct that contains I/O
   //port information, etc. Higher-level code should
   //not touch this.
   void *drive_data;
}drive_t;

void drive_register(drive_t *drive);
