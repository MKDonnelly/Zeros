#include <fs/mbr.h>

mbr_part_t mbr_partitions[4];

//Reads a 512 byte block and saves the partition table
//stored in the MBR.
void mbr_read_parttable(char *mbr){
  memcpy(mbr_partitions, mbr + mbr_part_number(1), sizeof(mbr_part_t) * 4);
}

mbr_part_t *get_mbr_entry(int num){
   //partitions numbers start at 1,
   //array indexes start at 0. subtract 1.
   return &mbr_partitions[num-1];
}
