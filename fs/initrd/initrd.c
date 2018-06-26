#include <fs/initrd/initrd.h>

#include <kernel/mm/heap.h>
#include <kernel/blkdev/blkdev.h>
#include <lib/types.h>
#include <lib/string.h>
#include <fs/vfs/fsmanager.h>
#include <fs/vfs/vnode.h>

#define INITRD_MAGIC 0x12345678

//This represents the initrd superblock
typedef struct initrd_sb{
   uint32_t magic;
   uint32_t entries_count;
}initrd_sb_t;

//An initrd inode. Name is the name of
//the entity, id is a unique number for this inode,
//offset is the offset FROM THE START of the initrd
//(NOT into memory). length is the length in bytes,
//flags is used to distinguish among a file and directory.
#define FILENAME_MAX_LEN 32
typedef struct initrd_inode{
   char name[FILENAME_MAX_LEN];
   uint32_t id;
   uint32_t offset;
   uint32_t length;
   uint32_t  flags;
}initrd_inode_t;

#define FS_INITRD 1
//Represents an initrd filesystem exported to fsmanager
static fstype_t initrd = {
      .fs_type = FS_INITRD,
      .fs_id = 0x11111111,
      .parent = NULL,  //we use the parent as the location
                       //of the initrd in memory.
      .check_type = NULL,
      .get_id = NULL,
      .open_inodes = NULL,
      .root_dir = {
         .name = "/",
         .flags = 0,
         .fs = 0, //Points to containing structure
         .inode = 0,

         .read = NULL,
         .write = NULL,
         .open = NULL,
         .close = NULL,
         .readdir = initrd_readdir,
         .finddir = initrd_finddir,
         .len = initrd_len,
      }, 
   };

#define FS_INITRD	1
void initrd_init(size_t *start){
   initrd_sb_t *head = (initrd_sb_t*)start;
   initrd.parent = start;
   initrd.root_dir.fs = start;

   initrd_inode_t *root_inode = k_malloc(sizeof(initrd_inode_t));
   strcmp(root_inode->name, "/");
   root_inode->id = 0;
   root_inode->offset = start;
   root_inode->len = head->entries_count;
   list_add( initrd.open_inodes, root_inode, 0 );

   fsmanager_add_active_fs(&initrd);
}

int initrd_read(fs_node_t *file, int offset, int len, char *buffer){

}


int initrd_write(fs_node_t *file, int offset, int len, char *buffer){

}

int initrd_open(fs_node_t *node, int flags){

}

int initrd_close(fs_node_t *node){

}

dirent_t *initrd_readdir(fs_node_t *dir, int index){
   if( strcmp(dir->name, "/") != 0 )
      return NULL;
   
   dirent_t *new_dirent = k_malloc(sizeof(dirent_t), 0);
   k_printf("Got %x\n", dir->fs);
   initrd_inode_t *inode = (initrd_inode_t*)((char*)dir->fs + 
                           sizeof(initrd_sb_t) + index * sizeof(initrd_inode_t));
   k_printf("Inode at %x\n", inode);
   strcpy(new_dirent->name, inode->name);
   new_dirent->inode = inode->id;

   return new_dirent;
}

int initrd_len(fs_node_t *node){

}

fs_node_t *initrd_finddir(fs_node_t *dir, char *name){
/*   int i = 0;
   initrd_inode_t *first_inode = (initrd_inode_t*)((char*)dir->fs +
                                                   sizeof(initrd_sb_t))
   while( i < dir->

   fs_node_t *new_node = k_malloc(sizeof(fs_node_t), 0);
   strcpy(new_node->name, name);
   new_node->flags = 0;
   new_node->fs = dir->fs;
   new_node->inode = */
}
