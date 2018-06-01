#include <drivers/ata/ata_pio.h>

#include <arch/x86/archx86.h>
#include <kernel/mm/heap.h>

#define SECTOR_SIZE 512

//Primary registers
#define PBUS_BASEREG 	0x1F0
//Bit 1 - Set to stop interrupts
//Bit 2 - Software reset of all ATA devices on bus
//Bit 3 - Read back higher order byte from last lba48 request
#define PBUS_CTRL_PORT 	0x3F6
   #define CTRL_MASK_INTS	(1 << 1)
   #define CTRL_RESET		(1 << 2)
   #define CTRL_HOB		(1 << 3)

//Secondary bus register
#define SBUS_BASEREG	0x1E8
#define SBUS_CTRL_PORT	0x3E6

//Registers for use by the first buf
//Data is sent to/from the drive here
#define DATA_P 		PBUS_BASEREG 
//Various errors 
#define ERROR_P 	(PBUS_BASEREG + 1)
#define FEATURE_P 	(PBUS_BASEREG + 1)
//Sector count. Dictates how many sectors to r/w
#define SCOUNT_P 	(PBUS_BASEREG + 2)
//LBAX: The linear block address of the block to
//start reading/writing at
#define LBA0_P 		(PBUS_BASEREG + 3)
#define LBA1_P		(PBUS_BASEREG + 4)
#define LBA2_P		(PBUS_BASEREG + 5)
//Bits 7, 6, and 5 must be 1, 0, 1.
//Bit 4 of this port selects the drive. 0 selects 
//drive 0 (master), 1 selects slave.
//Bits 3-0 selects head (just set to 0)
#define DSELECT_P 	(PBUS_BASEREG + 6)
   #define DSELECT_HEAD	(0b101 << 5)
   #define MASTER_DRV	(0 << 4)
   #define SLAVE_DRV	(1 << 4)
   #define LBA_MODE	(1 << 6)
//Bit 7 - Controller is running a command
//Bit 6 - Drive is ready to accept a command
//Bit 5 - write fault
//Bit 4 - Seek complete
//Bit 3 - Buffer requires servicing
//Bit 2 - Disk read corrected
//Bit 1 - set to 1 on every revolution
//Bit 0 - Previous command errored out
#define STAT_P		(PBUS_BASEREG + 7)   //Read only
   //Status port masks
   #define STAT_ERR	(1 << 0)
   #define STAT_DRQ	(1 << 3)
   #define STAT_DRDY	(1 << 6)
   #define STAT_BSY	(1 << 7)

//Comand register. Note that this is the same as
//STAT_P: STAT_P is for reading, CMD_P is for writing
#define CMD_P		(PBUS_BASEREG + 7)   //Write only
   //Commands that can be send to CMD_P.
   //EXT versions are for LBA48
   //Read sectors with retry
   #define READ_C	0x20
   //Read sectors without retry
   #define READNT_C	0x21
   #define READ_DMA_C	0XC8
   //Write sectors with retry
   #define WRITE_C 	0x30
   //write sectors without retry
   #define WRITENT_C	0x31
   #define WRITE_DMA_C	0xCA
   #define CFLUSH_C	0xE7
   #define SEND_CMD_C	0xA0
   #define IDENT_C	0xEC

   #define READ_EXT_C		0x24
   #define READ_DMA_EXT_C 	0x25
   #define WRITE_EXT_C		0x34
   #define WRITE_DMA_EXT_C	0x35
   #define CFLUSH_EXT_C		0xEA

   //Useful stuff in ident output
   #define IDENT_DEVTYPE 	0
   #define IDENT_CYLINDERS	2
   #define IDENT_HEADS		6
   #define IDENT_SECTORS	12
   #define IDENT_SERIAL		20
   #define IDENT_MODEL		54
   //Offsets 60 and 61 taken together gives the
   //total number of LBA28 sectors
   #define IDENT_LBA28_1	60
   #define IDENT_LBA28_2	61
   //Bit 10 is set if LBA48 is supported
   #define IDENT_LBA48		83
   #define IDENT_CAPBLTS	98
   //Offsets 100..103 taken as a uint64_t give the 
   //total number of LBA48 sectors
   #define IDENT_LBA48_1   	100
   #define IDENT_LBA48_2   	101
   #define IDENT_LBA48_3   	102
   #define IDENT_LBA48_4   	103
   #define IDENT_FLDVLD		106
   #define IDENT_MAX_LBA	120
   #define IDENT_CMDSET		164
   #define IDENT_MAX_LBA_EXT 	200

#define SCOUNT1_P	(PBUS_BASEREG + 8)
#define LBA3_P		(PBUS_BASEREG + 9)
#define LBA4_P		(PBUS_BASEREG + 10)
#define LBA5_P		(PBUS_BASEREG + 11)
#define CTRL_P 		(PBUS_BASEREG + 12)
#define ALTSTAT_P	(PBUS_BASEREG + 12)
#define DEVADDR_P	(PBUS_BASEREG + 13)



//According to the ATA PIO specifications, if we can 
//write a specific value to an ATA register (SECTOR_COUNT
//used here), then we may assume that an ide controller is present
//This value is purely arbitrary.
uint8_t ata_controller_present(){
   //0x10 here is nothing special. All we are looking for is that
   //the value sticks and can be read back. 
   //The port we write to may be artitrary as well
   portb_write( SCOUNT_P, 0x12 );
   return 0x12 == portb_read( SCOUNT_P );
}

//Reset all ATA drivers. We set bit 2 (SRST) in the control register
//to reset all drives. We also need to clear this since it is not
//done automatically
void ata_reset(){
   portb_write( PBUS_CTRL_PORT, (1 << 2) );
   portb_write( PBUS_CTRL_PORT, 0 );
}

//According to the ATA specs, we need to have a 400ns delay before
//reading the command byte to give it time to settle. We do this by
//reading the command byte 4 times, and then sending the 5th read as
//valid data.
uint8_t ata_read_stat(){
   for(int i = 0; i < 4; i++)
      (void)portb_read( CMD_P );
   return portb_read( CMD_P );
}

//bus == 0 -> primary, bus == 1 -> secondary
//drive == 0 -> master, drive == 1 -> slave
//Only primary bus supported
uint16_t *identify_drive( uint8_t bus, uint8_t drive ){

   //Select drive Drive
   if( drive == 0 ){
      portb_write( DSELECT_P, 0b1010000 | ( drive == 0 ?
                                          MASTER_DRV : SLAVE_DRV));
   }
   
   //Zero out ports 0x1F2-0x1F5
   portb_write( SCOUNT_P, 0 );
   portb_write( LBA0_P, 0 );
   portb_write( LBA1_P, 0 );
   portb_write( LBA2_P, 0 );
  
   //now send actual identify command 
   portb_write( CMD_P, IDENT_C );

   //If stat == 0, no drive exists
   int stat = portb_read( CMD_P );
   if( stat == 0 )
      return NULL;

   //now poll status port until the BSY bit (bit 7) clears
   while( portb_read( STAT_P ) & STAT_BSY );

   //now wait until the DRQ bit (bit 3) is set
   while( ! (portb_read( STAT_P ) & STAT_DRQ ) );

   //Allocate space to read in the identify data
   uint16_t *identify_data = k_malloc( SECTOR_SIZE, 0);

   //if err is not set, read in the data
   if( ! (portb_read( STAT_P ) & STAT_ERR ) ){
      //A sector size is in bytes, but we are reading in
      //2 byte, so divide by 2.
      for(int i = 0; i < (SECTOR_SIZE / 2); i++)
         identify_data[i] = portw_read( DATA_P );
   }

   return identify_data;
}

//drive - 0 for master, 1 for slave
char *ata_pio_read( uint8_t total_sectors, uint32_t lba, uint8_t drive ){
   
   //1. Select drive and send highest 4 bits of lba
   portb_write( DSELECT_P,  ((lba >> 24) & 0xF) | LBA_MODE | DSELECT_HEAD |
                            ( drive == 0 ? MASTER_DRV : SLAVE_DRV ));

   //2. Send the total number of sectors to read
   portb_write( SCOUNT_P, total_sectors );

   //3. Send bottom 8 bits of lba
   portb_write( LBA0_P, lba & 0xFF );

   //4. Send middle 8 bits of lba
   portb_write( LBA1_P, (lba >> 8) & 0xFF);

   //5. Send upper 8 bits of lba
   portb_write( LBA2_P, (lba >> 16) & 0xFF );

   //6. Send the read command
   portb_write( CMD_P, READ_C );

   //7. Wait for drive by polling BSY and DRQ bit
   while( portb_read( STAT_P ) & STAT_BSY );
   while( ! (portb_read( STAT_P ) & STAT_DRQ ) );

   //we can now read in a sector
   uint16_t *buffer = k_malloc( SECTOR_SIZE, 0);
   memset( buffer, SECTOR_SIZE, 0 ); 

   //Divide by 2 since we are reading 16 bits at a time,
   //and SECTOR_SIZE is in bytes 
   for(int i = 0; i < (SECTOR_SIZE/2); i++){
      buffer[i] = portw_read( DATA_P );
   }

   return buffer;
}


void ata_pio_write( uint8_t sectors, uint32_t lba, uint16_t *buf, 
                    uint8_t drive ){

   //1. Select drive and send highest 4 bits of lba
   portb_write( DSELECT_P,  ((lba >> 24) & 0xF) | LBA_MODE | DSELECT_HEAD |
                            ( drive == 0 ? MASTER_DRV : SLAVE_DRV ));

   //2. Send total number of sectors to read
   portb_write( SCOUNT_P, sectors );

   //3. Send bottom 8 bits of lba
   portb_write( LBA0_P, lba & 0xFF );

   //4. Send middle 8 bits of lba
   portb_write( LBA1_P, (lba >> 8) & 0xFF);

   //5. Send upper 8 bits of lba
   portb_write( LBA2_P, (lba >> 16) & 0xFF );

   //6. Send write command
   portb_write( CMD_P, WRITE_C );

   //7. Pool for BSY and DRQ bit before writing
   while( portb_read( STAT_P ) & STAT_BSY );
   while( ! (portb_read( STAT_P ) & STAT_DRQ ) );

   //we can now write in a sector 
   //Divide by 2 since we are writing 16 bits at a time, 
   for(int i = 0; i < (SECTOR_SIZE/2); i++){
      portw_write( DATA_P, buf[i] );
   }
}
