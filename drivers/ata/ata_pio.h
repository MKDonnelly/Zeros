#pragma once

/*    Example usage
   char *buf = ata_pio_read( 1, 0, 1 );
   for(int i = 0; i < 512; i++)
       buf[i] = 'Z';
   ata_pio_write( 1, 0, buf, 1 );
*/

#include <lib/types.h>

uint16_t *identify_drive(uint8_t buf, uint8_t drive );
char *ata_pio_read(uint8_t sector_count, uint32_t lba, uint8_t drive );
void ata_pio_write(uint8_t sector_count, uint32_t lba, uint16_t *buf, 
                   uint8_t drive);
