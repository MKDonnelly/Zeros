#pragma once

#include <lib/types.h>

#define MBR_PART_1	0x1BE //446 bytes in
#define MBR_PART_2	0x1CE //462 bytes in
#define MBR_PART_3	0x1DE //478 bytes in
#define MBR_PART_4	0x1EE //494 bytes in

static inline uint32_t mbr_part_number(uint8_t num){
   switch(num){
       case 1:
          return MBR_PART_1;
       case 2:
          return MBR_PART_2;
       case 3:
          return MBR_PART_3;
       case 4:
          return MBR_PART_3;
   }
}

//MBR Partition entry. Each entry is
//16 bytes in length
typedef struct{
   //0 for not bootable, 0x80 for bootable
   uint8_t is_bootable;

   //Starting head for CHS access
   uint8_t starting_head;

   //Starting sector for CHS access
   uint16_t starting_sector : 6;

   //Starting cylinder
   uint16_t starting_cylinder : 10;

   //System ID
   uint8_t sys_id;

   //Ending head
   uint8_t ending_head;

   //Ending sector
   uint16_t ending_sector : 6;

   //ending cylinder
   uint16_t ending_cylinder : 10;

   //Partition's starting LBA value
   uint32_t start_lba;

   //total number of sectors in partition
   uint32_t total_sectors;
} __attribute__((packed)) mbr_part_t;

void mbr_read_parttable(char *mbr);
mbr_part_t *get_mbr_entry(int num);
