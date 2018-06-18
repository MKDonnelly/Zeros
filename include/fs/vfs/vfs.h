#pragma once
#include <fs/vfs/fsmanager.h>

typedef struct fs_obj{
   char name[20];
   fstype_t *containing_fs;

   void (*read)(fstype_t *fs, int offset, char *buffer);
   void (*write)(fstype_t *fs, int offset, char *buffer);

   int blocks[20];
}fs_obj_t;

