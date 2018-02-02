#pragma once

#include <lib/types.h>

//Masks for fs_node.flags
#define FS_FILE         0x01
#define FS_DIRECTORY    0x02
#define FS_CHARDEVICE   0x03
#define FS_BLOCKDEVICE  0x04
#define FS_PIPE         0x05
#define FS_SYMLINK      0x06
#define FS_MOUNTPOINT   0x08

struct fs_node;
//These are function types used in fs_node_t.  They are used to specify
//the type of function that can be used within fs_node_t. The functions 
//inside of fs_node_t will change with the storage type.
typedef uint32_t (*read_type_t)(struct fs_node*,uint32_t,uint32_t,int8_t*);
typedef uint32_t (*write_type_t)(struct fs_node*,uint32_t,uint32_t,int8_t*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);

//TODO We should have a lookup by inode as well
typedef struct fs_node * (*readdir_type_t)(struct fs_node*,uint32_t);
typedef struct fs_node * (*finddir_type_t)(struct fs_node*,char *name);

#define FS_NODE_NAME_MAXLEN 32

//This structure represents a
//file system as a whole.
typedef struct fs_node{
   char name[FS_NODE_NAME_MAXLEN];
   uint32_t mask;
   uint32_t uid;
   uint32_t gid;
   uint32_t flags;
   uint32_t inode;
   uint32_t length;
   uint32_t impl;

   //These functions are indirectly
   //called by read_fs, write_fs...
   //They should not be called by 
   //anything outside of filesystem
   //code as fs_node->read
   read_type_t read;
   write_type_t write;
   open_type_t open;
   close_type_t close;
   readdir_type_t readdir;
   finddir_type_t finddir;
   struct fs_node *ptr;
} fs_node_t;

//This strucure is returned by readdir
//typedef struct dirent{
//   char name[FS_NODE_NAME_MAXLEN]; //Filename
//   uint32_t ino; //Inode number for file/dir
//} directory_t;

//The root file system
extern fs_node_t *fs_root;


//These are the generic fs_node_t operators. These will perform error
//checking before calling the specific function inside of the
//provided fs_node_t. These can be called outside of fs code.

//This function reads from the given file (the fs_node_t*) at the given
//offset and size, and places it in *buffer. THIS SHOULD REFERENCE A FILE!
//This function will do error checking to make sure that we do not read
//off the end of the file.
uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, int8_t *buffer);
uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, int8_t *buffer);
void open_fs(fs_node_t *node, uint8_t read, uint8_t write);
void close_fs(fs_node_t *node);

//This function will return information about a file/directory
//at the index of the directory. For example, if a directory
//has three files (file1,file2,file3), a call of readdir_fs(...,0)
//will return info about file1, readdir_fs(...,1) will return info
//about file2, and so on.
//struct dirent *readdir_fs(fs_node_t *node, uint32_t index);
fs_node_t *readdir_fs(fs_node_t *node, uint32_t index);
//Given the name of a file, try to locate it in the directory.
fs_node_t *finddir_fs(fs_node_t *node, char *name);
