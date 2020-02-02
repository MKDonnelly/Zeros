#pragma once

#include <lib/types.h>
#include <fs/vfs/vnode.h>

#define INITRD_MAGIC 0x12345678
#define NAME_MAX_LEN 24

typedef struct initrd_sb{
   uint32_t magic; //Must be INITRD_MAGIC
   uint32_t entries_offset;
   uint32_t entries_len;
}initrd_sb_t;

typedef struct initrd_entry{
   uint32_t entry_offset;
   uint32_t entry_len;
   char name[NAME_MAX_LEN];
}initrd_entry_t;

void initrd_init(size_t *start);
int initrd_read(initrd_sb_t *initrd, char *name, int len, char *buffer);



//int initrd_len(fs_node_t *node);
//int initrd_write(fs_node_t *file, int offset, int len, char *buffer);
//int initrd_open(fs_node_t *node, int flags);
//int initrd_close(fs_node_t *node);
//dirent_t *initrd_readdir(fs_node_t *dir, int index);
//fs_node_t *initrd_finddir(fs_node_t *dir, char *name);
