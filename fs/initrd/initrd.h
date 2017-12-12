#pragma once

#include <types.h>
#include <fs.h>
#include <kmalloc.h>
#include <vgacommon.h>

#define INITRD_MAGIC 0x12345678
#define FILENAME_MAX_LEN 32

//This represents the initrd header
struct initrd_header{
   uint32_t magic;
   uint32_t numFiles;
};

//This represents a file/directory in the initrd
//By design, inode will also be the offset in the
//array of initrd_object's in the initrd.
struct initrd_object{
   char name[FILENAME_MAX_LEN];
   uint32_t inode;
   uint32_t offset;
   uint32_t length;
   uint32_t  flags;
};

//This is a collection of information that
//is used throuought the initrd code.
struct cur_initrd{
   struct initrd_header *initrdHeader;
   struct initrd_object *initrdObjects;
   int numFiles;
} current_initrd;

//This is all a calling process should need to
//know. The fs_node_t interface is adequate for
//everything else.
fs_node_t *init_initrd(uint32_t*);

