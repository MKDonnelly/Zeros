#include <fs/initrd/initrd.h>

#include <kernel/mm/heap.h>
#include <kernel/blkdev/blkdev.h>
#include <lib/types.h>
#include <lib/string.h>
#include <fs/vfs/fsmanager.h>
#include <fs/vfs/vnode.h>
#include <lib/genericll.h>

#define INITRD_MAGIC 0x12345678

//An initrd inode. Name is the name of
//the entity, id is a unique number for this inode,
//offset is the offset FROM THE START of the initrd
//(NOT into memory). length is the length in bytes,
//flags is used to distinguish among a file and directory.
#define FILENAME_MAX_LEN 32
#define INITRD_FILE 0
#define INITRD_DIR 1
typedef struct initrd_inode{
   list_head_t inodes_list;
   uint32_t inode;
   uint32_t offset;
   uint32_t length;
   uint32_t type;
}initrd_inode_t;

typedef struct initrd_dirent{
   char name[128];
   uint32_t inode;
}initrd_dirent_t;

//This represents the initrd superblock
//It contains the inode describing root
typedef struct initrd_sb{
   uint32_t magic;
   initrd_inode_t root_inode;
   uint32_t inodes_offset;
   uint32_t inodes_len;
}initrd_sb_t;

#define FS_INITRD 1
//Represents an initrd filesystem exported to fsmanager
static fstype_t initrd = {
      .fs_type = FS_INITRD,
      .fs_id = 0x11111111, //hard coded for simplicity.
      .parent = NULL,  //Not used since this does not reside
                       //on a block device.
      .check_type = NULL, //not used
      .get_id = NULL, //not used
      .open_inodes = NULL, //linked list of open inodes
      .superblock = NULL, //pointer to superblock

      //The first inode in the superblock will be used to populate this
      //Code outside of the filesystem should grab this and use it to 
      //manage accessing the file system.
      .root_dir = { 
         .name = "/",
         .flags = 0,
         .fs = 0, //Points to containing structure (i.e. this fstype_t)
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
   initrd.superblock = (void*)start;
   initrd.root_dir.fs = &initrd;

   //Initilize list in open_inodes part of the fstype_t
   initrd.open_inodes = (void*)list_create(initrd_inode_t, inodes_list);

   //Put in the inode for the root filesystem. Copy it from the initrd.
//   initrd_inode_t *root_inode = k_malloc(sizeof(initrd_inode_t), 0);
//   memcpy(root_inode, &(((initrd_sb_t*)initrd.superblock)->root_inode), 
//           sizeof(initrd_inode_t));

   list_pushfront((list_t*)initrd.open_inodes, 
                  &(((initrd_sb_t*)initrd.superblock)->root_inode));
   //list_pushfront((list_t*)initrd.open_inodes, root_inode);

   fsmanager_add_active_fs(&initrd);
}

//Find the inode in the initrd. This inode is not in the open_inodes list,
//and is intended to be used when first managing a file. Any node
//on the initrd can be passed, since they all contain a pointer to the
//fstype_t structure.
initrd_inode_t *initrd_find_closed_inode(fs_node_t *node_on_initrd, 
                                         int inode){
   initrd_sb_t *initrd_sb = node_on_initrd->fs->superblock;
   initrd_inode_t *inodes = (initrd_inode_t*)((char*)initrd_sb + 
                                               initrd_sb->inodes_offset);

   for(int i = 0; i < initrd_sb->inodes_len; i++){
      if( inodes->inode == inode )
         return inodes;
      inodes++;
   }
}

//Searches for the inode of the given fs_node_t. THE INODE MUST HAVE
//BEEN OPENED, ELSE THIS WILL NOT FIND IT.
initrd_inode_t *initrd_find_open_inode(fs_node_t *node){
   list_foreach((list_t*)node->fs->open_inodes, iter_node, initrd_inode_t){
      if( iter_node->inode == node->inode )
         return iter_node;
   }
}

int initrd_read(fs_node_t *file, int foffset, int len, char *buffer){
   //First, find the inode. It must be open by now.
   initrd_inode_t *inode = initrd_find_open_inode(file);

   //TODO bounds checking
   //Start at the head of the initrd, then jump to the offset of the 
   //file within the initrd. Then advance however far into the file
   //we were asked to.
   memcpy(buffer, (char*)file->fs->superblock + inode->offset + foffset,
          len);
}


int initrd_write(fs_node_t *file, int offset, int len, char *buffer){

}

int initrd_open(fs_node_t *node, int flags){
}

int initrd_close(fs_node_t *node){

}

dirent_t *initrd_readdir(fs_node_t *dir, int index){
   //Allocate space for new directory entry
   dirent_t *new_dirent = k_malloc(sizeof(dirent_t), 0);

   //Lookup inode for directory, and then get a pointer
   //to its contents (superblock is start of initrd)
   initrd_inode_t *dir_inode = initrd_find_open_inode(dir);

   //Asked to read past end of directory. Return NULL.
   if( index >= dir_inode->length )
      return NULL;
  
   //To find the specific directory entry, start at the head of the initrd.
   //Then add the offset to the directory contents. Since each entry
   //has constant width, we can quickly jump to the n'th one by multiplying
   //the index times their size.
   initrd_dirent_t *dir_entry = 
                  (initrd_dirent_t*)((char*)dir->fs->superblock + 
                      dir_inode->offset + index * sizeof(initrd_dirent_t));

   //copy over info
   strcpy(new_dirent->name, dir_entry->name);
   new_dirent->inode = dir_entry->inode;

   return new_dirent;
}


//Find length of an open inode.
int initrd_len(fs_node_t *node){
   //Search for the inode in the list
   initrd_inode_t *found = NULL;
   found = initrd_find_open_inode(node);

   if( found != NULL ){
      return found->length;
   }
/*   list_foreach( (list_t*)node->fs->open_inodes, 
                                 iter_node, initrd_inode_t ){
      if( iter_node->inode == node->inode )
         return iter_node->length;
   }*/
   //Node is not opened or does not exist
   return -1;
}


fs_node_t *initrd_finddir(fs_node_t *dir, char *name){

   //Find inode of directory
   initrd_inode_t *dir_inode = initrd_find_open_inode(dir);
   
   //Get pointer to directory entries
   initrd_dirent_t *dir_entries = 
         (initrd_inode_t*)((char*)dir->fs->superblock + dir_inode->offset);

   for(int i = 0; i < dir_inode->length; i++){
      if( strcmp(name, dir_entries[i].name) == 0 ){
         //Create a new inode for the file. This has the effect
         //of opening the file. Add it to open inodes
         initrd_inode_t *new_file_inode = 
                 initrd_find_closed_inode(dir, dir_entries[i].inode);
         list_pushfront( (list_t*)dir->fs->open_inodes, new_file_inode );

         //Now create a fs_node_t which will refer to the inode created
         fs_node_t *new_file = k_malloc(sizeof(fs_node_t), 0);
         strcpy(new_file->name, dir_entries[i].name);
         new_file->type = 0; //file, for now
         new_file->fs = dir->fs; //file must be on same drive as directory
         new_file->inode = dir_entries[i].inode;
         new_file->read = initrd_read;
         new_file->len = initrd_len;

         return new_file;
      }
   }
   return NULL;
}
