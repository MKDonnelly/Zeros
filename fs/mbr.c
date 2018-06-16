#include <fs/mbr.h>
#include <lib/string.h>
#include <kernel/mm/heap.h>

typedef struct mbr_info{
   mbr_part_t mbr_partitions[MAX_MBR_PARTITIONS];
}mbr_info_t;

//Reads the first block of the drive and saves the MBR table
//within the drive_t*
void mbr_setup_parttable(drive_t *drive){
   //Read in the first sector
   char *buffer = k_malloc( drive->blksize, 0 );
   drive->read_lba( drive, buffer, 0, 1 );

   mbr_info_t *mbr_info = k_malloc(sizeof(mbr_info_t), 0);
   memcpy(mbr_info, buffer + MBR_PARTINFO_START, sizeof(mbr_part_t)*4);
   drive->partitions.data = mbr_info;
   drive->partitions.count = MAX_MBR_PARTITIONS;

   //Setup functions for accessing MBR data
   drive->partitions.part_start = mbr_get_partstart_lba;
   drive->partitions.part_len = mbr_get_partsize;
}

uint32_t mbr_get_partstart_lba(drive_t *drive, int part_number){
   //Subtract 1 since partitions numbers start at 1
   return ((mbr_info_t*)drive->partitions.data)->mbr_partitions[part_number-1].start_lba;
}

uint32_t mbr_get_partsize(drive_t *drive, int part_number){
   return ((mbr_info_t*)drive->partitions.data)->mbr_partitions[part_number-1].total_sectors;
}
