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

typedef struct fs_node fs_node_t;
//These are function types used in fs_node_t.  They are used to specify
//the type of function that can be used within fs_node_t. The functions 
//inside of fs_node_t will change with the storage type.
//TODO incorporate a current position for the vnode since 
//     offset is ignored
typedef int (*vnode_read_t)(fs_node_t *self, int offset, 
                            int len, char *buffer);
typedef int (*vnode_write_t)(fs_node_t *self, int offset, 
                            int len, char *buffer);
typedef int (*vnode_open_t)(fs_node_t *self, char *name, int flags);
typedef int (*vnode_close_t)(fs_node_t *self);
typedef int (*vnode_len_t)(fs_node_t *self);

typedef struct dirent{
   char name[128];
}dirent_t;

typedef dirent_t *(*vnode_readdir_t)(fs_node_t *self, int index);
typedef fs_node_t *(*vnode_finddir_t)(fs_node_t *self, char *name);

#define FS_NODE_NAME_MAXLEN 32

//Most of the actual data is stored in the inode. The inode is
//filesystem specific, and this provides a generic interface to it.
//This basically associates a name and operations with an inode.
typedef struct fstype fstype_t; //defined in fsmanager.h
typedef struct fs_node{
   char name[FS_NODE_NAME_MAXLEN];
   int flags; 
   char type;
   fstype_t *fs;

   //Pointer to fs-specific data structure.
   void *inode;

   //These point to specific file system functions.
   vnode_read_t read;
   vnode_write_t write;
   vnode_open_t open;
   vnode_close_t close;
   vnode_readdir_t readdir;
   vnode_finddir_t finddir;
   vnode_len_t len;
} fs_node_t;

//system calls
int sys_open(char *name);
int sys_write(int fd, char *buf, int len);
int sys_read(int fd, char *ubuf, int len);
