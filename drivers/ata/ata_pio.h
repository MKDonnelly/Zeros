#pragma once

#include <lib/types.h>

//This is 256 16-bit values, for 512 bits total
#define SECTOR_SIZE 256

//Primary bus base registers
#define ATA_PDRIVE_BAR0_P 0x1F0
#define ATA_PDRIVE_BAR1_P 0x3F6

//Secondary bus base registers
#define ATA_SDRIVE_BAR0_P 0x170
#define ATA_SDRIVE_BAR1_P 0x376

#define ATA_PIO_DATA_P ATA_PDRIVE_BAR0_P
#define ATA_PIO_ERROR_P (ATA_PIO_DATA_P + 1)
#define ATA_PIO_FEATURE_P ATA_PIO_ERROR_P
#define ATA_PIO_SECTORS_P (ATA_PIO_DATA_P + 2)
#define ATA_PIO_LBA0_P (ATA_PIO_DATA_P + 3)
#define ATA_PIO_LBA1_P (ATA_PIO_DATA_P + 4)
#define ATA_PIO_LBA2_P (ATA_PIO_DATA_P + 5)

//Send 0xA0 for master, 0xB0 for slave
#define ATA_PIO_DRIVE_P (ATA_PIO_DATA_P + 6)

//Also cometimes called status port
#define ATA_PIO_CMD_P (ATA_PIO_DATA_P + 7)

#define ATA_PIO_STAT_P ATA_PIO_CMD_P
#define ATA_PIO_SCOUNT1_P (ATA_PIO_DATA_P + 8)
#define ATA_PIO_LBA3_P (ATA_PIO_DATA_P + 9)
#define ATA_PIO_LBA4_P (ATA_PIO_DATA_P + 10)
#define ATA_PIO_LBA5_P (ATA_PIO_DATA_P + 11)
#define ATA_PIO_CTRL_P ATA_PDRIVE_BAR1_P 

//Commands
#define ATA_PIO_READ_C 0x20
#define ATA_PIO_WRITE_C 0x30
#define ATA_PIO_CFLUSH_C 0xE7
#define ATA_PIO_IDENTIFY_C 0xEC

uint16_t *identify_drive(uint8_t buf, uint8_t drive );
uint16_t *ata_pio_read_sector( uint8_t sector_count, uint32_t lba );
void ata_pio_write_sector(uint8_t sector_count, uint32_t lba, uint16_t *buf);
