#pragma once

//Superblock used by all filesystems
typedef struct sblock{
   drive_t *device;
   uint32_t block_size;
   uint32_t fs_type;
   
}sblock_t;
