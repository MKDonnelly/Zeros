#pragma once
#include <lib/types.h>

//The common interface that all drives support.
typedef struct drive{
   void (*drive_write_lba)(struct drive *drive, char *buffer,
                           uint32_t lba, uint32_t size);
   void (*drive_read_lba)(struct drive *drive, char *buffer,
                           uint32_t lba, uint32_t size);
   void (*drive_reset)();

   //Partition information. Used by partition functions.
   uint32_t (*drive_part_lba_start)(struct drive *drive, 
                                    int partition_number);
   uint32_t (*drive_part_size)(struct drive *drive,
                               int partition_number);
   void *partition_data;

   //Various pieces of information about the drive
   uint32_t drive_max_lba;
   uint32_t drive_block_size;

   //This will store data specific to a drive.
   //i.e. the ATA pio mode drive may have this point
   //to a ata_pio_drive_t struct that contains I/O
   //port information, etc. Higher-level code should
   //not touch this.
   void *drive_data;
}drive_t;

