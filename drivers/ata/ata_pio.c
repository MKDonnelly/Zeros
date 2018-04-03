#include <drivers/ata/ata_pio.h>

//According to the ATA PIO specifications, if we can 
//write a specific value to an ATA register (SECTOR_COUNT
//used here), then we may assume that an ide controller is present
//This value is purely arbitrary.
uint8_t ata_controller_present(){
   portb_write( ATA_PIO_SECTORS_P, 0x10 );
   return 0x10 == portb_read( ATA_PIO_SECTORS_P );
}

//Reset all ATA drivers. We set bit 2 (SRST) in the control register
//to reset all drives. We also need to clear this since it is not
//done automatically
void ata_reset(){
   portb_write( ATA_PIO_CTRL_P, 0b100 );
   portb_write( ATA_PIO_CTRL_P, 0 );
}

//According to the ATA specs, we need to have a 400ns delay before
//reading the command byte to give it time to settle. We do this by
//reading the command byte 4 times, and then sending the 5th read as
//valid data.
uint8_t ata_read_stat(){
   for(int i = 0; i < 4; i++)
      (void)portb_read( ATA_PIO_CMD_P );
   return portb_read( ATA_PIO_CMD_P );
}

//bus == 0 -> primary, bus == 1 -> secondary
//drive == 0 -> master, drive == 1 -> slave
uint16_t *identify_drive( uint8_t bus, uint8_t drive ){

  //Select master drive
  if( drive == 0 )
     portb_write( ATA_PIO_DRIVE_P, 0xA0 );

  //Zero out ports 0x1F2-0x1F5
  portb_write( ATA_PIO_SECTORS_P, 0 );
  portb_write( ATA_PIO_LBA0_P, 0 );
  portb_write( ATA_PIO_LBA1_P, 0 );
  portb_write( ATA_PIO_LBA2_P, 0 );
  
  //now send actual identify command (0xEC)
  portb_write( ATA_PIO_CMD_P, ATA_PIO_IDENTIFY_C );

  //If stat == 0, no drive exists
  int stat = portb_read( ATA_PIO_CMD_P );
  if( stat == 0 )
     return NULL;

  //now poll status port until the BSY bit (bit 7) clears
  while( portb_read( ATA_PIO_CMD_P ) & 0x80 );

  //now wait until the DRQ bit (bit 3) is set
  while( ! (portb_read( ATA_PIO_CMD_P ) & 0x8 ) );

  uint16_t *identify_data = k_malloc( kernel_heap, sizeof(uint16_t) * SECTOR_SIZE, 0);

  //if err is not set, read in the data
  if( ! (portb_read( ATA_PIO_CMD_P ) & 0x1 ) ){
     for(int i = 0; i < SECTOR_SIZE; i++)
        identify_data[i] = portw_read( ATA_PIO_DATA_P );
  }

  return identify_data;
}

uint16_t *ata_pio_read_sector( uint8_t total_sectors, uint32_t lba ){

  //1. Select drive and send highest 4 bits of lba
  portb_write( ATA_PIO_DRIVE_P, 0xE0 | ( (lba >> 24) & 0xF ) );
  //2. Send the total number of sectors to read
  portb_write( ATA_PIO_SECTORS_P, total_sectors );
  //3. Send bottom 8 bits of lba
  portb_write( ATA_PIO_LBA0_P, lba & 0xFF );
  //4. Send middle 8 bits of lba
  portb_write( ATA_PIO_LBA1_P, (lba >> 8) & 0xFF);
  //5. Send upper 8 bits of lba
  portb_write( ATA_PIO_LBA2_P, (lba >> 16) & 0xFF );
  //6. Send the read command
  portb_write( ATA_PIO_CMD_P, ATA_PIO_READ_C );
  //7. Wait for drive by polling BSY and DRQ bit

  //BSY is bit 7
  while( portb_read( ATA_PIO_CMD_P ) & 0x80 );

  //now wait until the DRQ bit (bit 3) is set
  while( ! (portb_read( ATA_PIO_CMD_P ) & 0x8 ) );

  //we can now read in a sector
  uint16_t *buffer = k_malloc( kernel_heap, sizeof(uint16_t) * SECTOR_SIZE, 0);
  for(int i = 0; i < SECTOR_SIZE; i++){
     buffer[i] = portw_read( ATA_PIO_DATA_P );
  }

  return buffer;
}

void ata_pio_write_sector( uint8_t sectors, uint32_t lba, uint16_t *buf ){

  //1. Select drive and send highest 4 bits of lba
  portb_write( ATA_PIO_DRIVE_P, 0xE0 | ( (lba >> 24) & 0x0F ) );
  //2. Send total number of sectors to read
  portb_write( ATA_PIO_SECTORS_P, sectors );
  //3. Send bottom 8 bits of lba
  portb_write( ATA_PIO_LBA0_P, lba & 0xFF );
  //4. Send middle 8 bits of lba
  portb_write( ATA_PIO_LBA1_P, (lba >> 8) & 0xFF);
  //5. Send upper 8 bits of lba
  portb_write( ATA_PIO_LBA2_P, (lba >> 16) & 0xFF );
  //6. Send write command
  portb_write( ATA_PIO_CMD_P, ATA_PIO_WRITE_C );
  //7. Pool for BSY and DRQ bit before writing

  //now poll status port until the BSY bit (bit 7) clears
  while( portb_read( ATA_PIO_CMD_P ) & 0x80 );

  //now wait until the DRQ bit (bit 3) is set
  while( ! (portb_read( ATA_PIO_CMD_P ) & 0x8 ) );

  //we can now write in a sector 
  for(int i = 0; i < SECTOR_SIZE; i++){
     portw_write( ATA_PIO_DATA_P, buf[i] );
  }
}
