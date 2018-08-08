#include <drivers/ata/ata_pio.h>

#include <arch/x86/archx86.h>
#include <kernel/mm/heap.h>

//TODO Add in LBA48 support

#define BLOCK_SIZE	512

//Primary registers
#define PRIMARY_BUS_BASEREG 	0x1F0
//Bit 1 - Set to stop interrupts
//Bit 2 - Software reset of all ATA devices on bus
//Bit 3 - Read back higher order byte from last lba48 request
#define PRIMARY_BUS_CTRLREG 	0x3F6
   #define CTRL_MASK_INTS	(1 << 1)
   #define CTRL_RESET		(1 << 2)
   #define CTRL_HOB		(1 << 3)

//Secondary bus register
#define SBUS_BASEREG	0x1E8
#define SBUS_CTRL_PORT	0x3E6

     //These are offsets (in bytes) from the drive's base register
//Data is sent to/from the drive here
#define DATA_P 		0
//Various errors 
#define ERROR_P 	1
#define FEATURE_P 	1
//Sector count. Dictates how many sectors to r/w
#define SCOUNT_P 	2
//LBAX: The linear block address of the block to
//start reading/writing at
#define LBA0_P 		3
#define LBA1_P		4
#define LBA2_P		5
//Bits 7, 6, and 5 must be 1, 0, 1.
//Bit 4 of this port selects the drive. 0 selects
//drive 0 (master), 1 selects slave.
//Bits 3-0 selects head (just set to 0)
#define DSELECT_P 	6
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
#define STAT_P		7   //Read only
   //Status port masks
   #define STAT_ERR	(1 << 0)
   #define STAT_DRQ	(1 << 3)
   #define STAT_DRDY	(1 << 6)
   #define STAT_BSY	(1 << 7)

//Comand register. Note that this is the same as
//STAT_P: STAT_P is for reading, CMD_P is for writing
#define CMD_P		7   //Write only
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

#define SCOUNT1_P	8
#define LBA3_P		9
#define LBA4_P		10
#define LBA5_P		11
#define CTRL_P 		12
#define ALTSTAT_P	12
#define DEVADDR_P	13

//Structure representing an ata pio drive
//This is included in the drive_data section
//of the drive_t structure.
typedef struct ata_pio_drive{
   //Base register for operations
   uint16_t base_register;
   uint16_t ctrl_register;   
   
   //0 for master, 1 for slave
   uint8_t master_or_slave;

   //the bus number starting from 0 
   uint8_t bus;
}ata_pio_drive_t;

//Used to determine if we need to switch drives.
ata_pio_drive_t *current_drive;

void ata_enumerate(){
   drive_t *ata_drive = ata_pio_create_drive(0, 1);
   mbr_setup_parttable(ata_drive);
   drive_register(ata_drive);   
}

//Returns a drive_t* to a structure representing this drive
//Currently only supports the primary bus
drive_t *ata_pio_create_drive(int bus, int drive_number){
   drive_t *drive = k_malloc(sizeof(drive_t), 0);
  
   //Set the various functions 
   drive->write_lba = ata_pio_write;
   drive->read_lba = ata_pio_read;
   drive->reset = ata_pio_reset;
   drive->blksize = BLOCK_SIZE;

   //Create a structure representing the drive
   ata_pio_drive_t *ata_pio_drive = k_malloc(sizeof(ata_pio_drive_t), 0);
   ata_pio_drive->bus = 0; //Currently only support primary bus
   ata_pio_drive->base_register = PRIMARY_BUS_BASEREG;
   ata_pio_drive->ctrl_register = PRIMARY_BUS_CTRLREG;
   ata_pio_drive->master_or_slave = drive_number ? SLAVE_DRV : MASTER_DRV;
   drive->drive_data = ata_pio_drive;

   //Have identify populate the drive_max_lba field
   ata_pio_reset( drive );
   ata_pio_identify_drive( drive );

   return drive;
}

//According to the ATA specs, we need to have a 400ns delay before
//reading the command byte to give it time to settle. We do this by
//reading the command byte 4 times, and then sending the 5th read as
//valid data.
uint8_t ata_pio_read_stat(drive_t *drive){
   ata_pio_drive_t *ata_pio = (ata_pio_drive_t*)drive->drive_data;

   for(int i = 0; i < 4; i++)
      (void)portb_read( ata_pio->base_register + CMD_P );
   return portb_read( ata_pio->base_register + CMD_P );
}


//According to the ATA PIO specifications, if we can 
//write a specific value to an ATA register (SECTOR_COUNT
//used here), then we may assume that an ide controller is present
//This value is purely arbitrary.
/*uint8_t ata_controller_present(int bus, int drive){
   //0x10 here is nothing special. All we are looking for is that
   //the value sticks and can be read back. 
   //The port we write to may be artitrary as well
   portb_write( SCOUNT_P, 0x12 );
   return 0x12 == portb_read( SCOUNT_P );
}*/

void ata_pio_select(drive_t *drive){
   ata_pio_drive_t *ata_pio = (ata_pio_drive_t*)drive->drive_data;   

   //Select drive. We must do this before using it.
   portb_write(ata_pio->base_register + DSELECT_P, 0b1010000 | 
               ata_pio->master_or_slave);
  
   //Add some delay so that the drive has time to settle
   (void)ata_pio_read_stat(drive);
}

//Reset all ATA drivers. We set bit 2 (SRST) in the control register
//to reset all drives. We also need to clear this since it is not
//done automatically
void ata_pio_reset(drive_t *drive){
   ata_pio_drive_t *ata_pio = (ata_pio_drive_t*)drive->drive_data;

   portb_write( ata_pio->ctrl_register, CTRL_RESET );
   //Clear the reset bit, since the hardware will not do it.
   portb_write( ata_pio->ctrl_register, 0 );
}


//Gather useful information about drive and store it into the
//drive structure
void ata_pio_identify_drive( drive_t *drive ){
   ata_pio_drive_t *ata_pio = (ata_pio_drive_t*)drive->drive_data;

   //Select drive
   ata_pio_select(drive);   
   
   //Zero out the sector count and LBA ports
   portb_write( ata_pio->base_register + SCOUNT_P, 0 );
   portb_write( ata_pio->base_register + LBA0_P, 0 );
   portb_write( ata_pio->base_register + LBA1_P, 0 );
   portb_write( ata_pio->base_register + LBA2_P, 0 );
  
   //now send actual identify command 
   portb_write( ata_pio->base_register + CMD_P, IDENT_C );

   //If stat == 0, no drive exists
   if( portb_read( ata_pio->base_register + CMD_P ) == 0 )
      return;

   //now poll status port until the BSY bit (bit 7) clears
   while( portb_read( ata_pio->base_register + STAT_P ) & STAT_BSY );

   //now wait until the DRQ bit (bit 3) is set
   while( !(portb_read( ata_pio->base_register + STAT_P ) & STAT_DRQ));

   //Allocate space to read in the identify data
   uint16_t *identify_data = k_malloc(BLOCK_SIZE, 0);

   //if err is not set, read in the data
   if( !(portb_read( ata_pio->base_register + STAT_P ) & STAT_ERR)){
      //A sector size is in bytes, but we are reading in
      //2 byte, so divide by 2.
      for(int i = 0; i < (BLOCK_SIZE / 2); i++)
         identify_data[i] = portw_read( ata_pio->base_register + DATA_P);
   }

   //Grab some of the more useful information from the buffer
   
   //Get the maximum LBA value for the drive.
   drive->maxlba = (identify_data[IDENT_LBA28_2] << 16) |
                           identify_data[IDENT_LBA28_1];
}


void ata_pio_read( drive_t *drive,  size_t lba, 
                   size_t blocks, char *buf){
   //THIS IS CRUCIAL! We read 16 bits at a time, so our array
   //MUST be an array of 16 bit members!
   uint16_t *buffer = (uint16_t*)buf;

   ata_pio_drive_t *ata_pio = (ata_pio_drive_t*)drive->drive_data;   

   //Wait for the drive to be ready before we write. Without this,
   //our write will get lost
   while( ata_pio_read_stat(drive) & STAT_BSY );
   while( ata_pio_read_stat(drive) & STAT_DRQ );

   //1. Select drive and send highest 4 bits of lba
   portb_write( ata_pio->base_register + DSELECT_P,  
                ((lba >> 24) & 0xF) | LBA_MODE | DSELECT_HEAD |
                ata_pio->master_or_slave);

   //2. Send the total number of sectors to read
   portb_write( ata_pio->base_register + SCOUNT_P, blocks );

   //3. Send bottom 8 bits of lba
   portb_write( ata_pio->base_register + LBA0_P, lba & 0xFF );

   //4. Send middle 8 bits of lba
   portb_write( ata_pio->base_register + LBA1_P, (lba >> 8) & 0xFF);

   //5. Send upper 8 bits of lba
   portb_write( ata_pio->base_register + LBA2_P, (lba >> 16) & 0xFF );

   //6. Send the read command
   portb_write( ata_pio->base_register + CMD_P, READ_C );

   //7. Wait for drive by polling BSY and DRQ bit
   //NOTE: We wait for the DRQ bit to be set here, while at the
   //      start we waited for it to be clear.
   while( ata_pio_read_stat(drive) & STAT_BSY );
   while( ! (ata_pio_read_stat(drive) & STAT_DRQ ) );

   //Divide by 2 since we are reading 16 bits at a time,
   //and SECTOR_SIZE is in bytes 
   for(int i = 0; i < (BLOCK_SIZE/2); i++){
      buffer[i] = portw_read( ata_pio->base_register + DATA_P );
   }
}


void ata_pio_write(drive_t *drive, size_t lba, 
                   size_t blocks, char *buf){ 
   //THIS IS CRUCIAL! We write 16 bits at a time, so our array
   //MUST be an array of 16 bit members!
   uint16_t *buffer = (uint16_t*)buf;

   ata_pio_drive_t *ata_pio = (ata_pio_drive_t*)drive->drive_data;

   //Wait for the drive to be ready before we write. Without this,
   //our write will get lost
   while( ata_pio_read_stat(drive) & STAT_BSY );
   while( ata_pio_read_stat(drive) & STAT_DRQ );

   //1. Select drive and send highest 4 bits of lba
   portb_write( ata_pio->base_register + DSELECT_P,  
                ((lba >> 24) & 0xF) | LBA_MODE | DSELECT_HEAD |
                ata_pio->master_or_slave);

   //2. Send total number of sectors to read
   portb_write( ata_pio->base_register + SCOUNT_P, blocks);

   //3. Send bottom 8 bits of lba
   portb_write( ata_pio->base_register + LBA0_P, lba & 0xFF );

   //4. Send middle 8 bits of lba
   portb_write( ata_pio->base_register + LBA1_P, (lba >> 8) & 0xFF);

   //5. Send upper 8 bits of lba
   portb_write( ata_pio->base_register + LBA2_P, (lba >> 16) & 0xFF );

   //6. Send write command
   portb_write( ata_pio->base_register + CMD_P, WRITE_C );

   //7. Poll for BSY and DRQ bit before writing
   while( ata_pio_read_stat(drive) & STAT_BSY );
   //NOTE: Here we wait for the Drive Ready Queue bit to be
   //      set, in contrast to the start of this function
   //      when we waited for it to be clear.
   while( ! (ata_pio_read_stat(drive) & STAT_DRQ ) );

   //we can now write in a sector 
   //Divide by 2 since we are writing 16 bits at a time, 
   for(int i = 0; i < (BLOCK_SIZE/2); i++){
      portw_write( ata_pio->base_register + DATA_P, buffer[i] );
   }
}
