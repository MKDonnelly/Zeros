#pragma once
#include <fs/mbr.h>

/*    Example usage
   char *buf = ata_pio_read( 1, 0, 1 );
   for(int i = 0; i < 512; i++)
       buf[i] = 'Z';
   ata_pio_write( 1, 0, buf, 1 );
*/

#include <lib/types.h>
#include <kernel/blkdev/drive.h>

void ata_enumerate();
//Returns a drive_t* given the bus and drive number.
//Returns NULL if the drive does not exist.
drive_t *ata_pio_create_drive(int bus, int drive);

void ata_pio_identify_drive(drive_t *drive);
void ata_pio_read(drive_t *drive, size_t lba, 
                  size_t blocks, char *buf);
void ata_pio_write(drive_t *drive, size_t lba, 
                   size_t blocks, char *buf);
void ata_pio_reset(drive_t *drive);
void ata_pio_select(drive_t *drive);
