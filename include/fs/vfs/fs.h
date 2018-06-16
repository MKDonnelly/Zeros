#pragma once

#include <fs/vfs/vnode.h> 

//Represents common functions that any file system
//must implement
typedef struct fstype{
   int type_id;
   int (*check_type)(char *start_block);
   void *(*get_superblock)();
   uint32_t (*get_id)(char *buf);
   fs_node_t *(*get_node)(char *name);
}fstype_t;

