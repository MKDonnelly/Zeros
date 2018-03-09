#include <fs/initrd/initrd.h>

//This does the job of reading a file from the initrd
//*node should be a pointer to a fs_node_t FILE
//Since we designed a file/dir in the initrd to have an
//inode corresponding to its index, we can place the index
//to get the initrd_object from the initrd header

//initrd_close, initrd_readfile, and initrd_writefile
//are used to implement close, read, and write for each
//file node returned by either initrd_readdir or initrd_finddir

//initrd_close, initrd_readdir and initrd_finddir are used to implement
//close, readdir, finddir for the initrd root node. initrd_close works
//for both files and directories.

void initrd_close(fs_node_t *node){
   //kfree( node );
   k_free( kernel_heap, node );
   node = NULL;
}

//TODO: Make some meaningful error codes to be returned
static uint32_t initrd_readfile(fs_node_t *node, uint32_t offset, uint32_t size, int8_t *buffer){

   struct initrd_object *obj = &current_initrd.initrdObjects[node->inode];

   //To find the start of the file in memory, find the absolute 
   //memory address of the initrd (from the initrdHeader) and add
   //the offset from the header stored in the individual initrdObject
   //Also add a provided offset within the file.
   int32_t fileLocation = (uint32_t)current_initrd.initrdHeader + obj->offset + offset;
   //We want an 8-bit pointer, since we will
   //be reading 1 byte values
   int8_t *ptr = (int8_t*)fileLocation;

   //Now we can start reading!
   int i;
   for(i = 0; i < size && i < node->length; i++){
      buffer[i] = ptr[i];
   }

   //Make sure the string read is terminated by a null
   //TODO change this as we will not always be reading strings
   buffer[i] = 0; 
   return 0;
}


//Writes to a file in the initrd. One major limitation: it can
//not expand a file; if we have a 100 byte file, we can only
//write to those 100 bytes, period. Later on, we will design a
//more complicated ramfs to allow expansion of files.
static uint32_t initrd_writefile(fs_node_t *node, uint32_t offset, uint32_t size, int8_t *buffer){

   struct initrd_object *obj = &current_initrd.initrdObjects[node->inode];

   //To find the start of the file in memory, find the absolute 
   //memory address of the initrd (from the initrdHeader) and add
   //the offset from the header stored in the individual initrdObject
   //Also add a provided offset within the file.
   int32_t fileLocation = (uint32_t)current_initrd.initrdHeader + obj->offset + offset;
   //We want an 8-bit pointer, since we will
   //be reading 1 byte values
   int8_t *ptr = (int8_t*)fileLocation;

   //Now we can start writing!
   int i;
   for(i = 0; i < size && i < node->length; i++){
      ptr[i] = buffer[i];
   }
   return 0;
}

//This function reads the given directory within the
//initrd and returns a fs_node_t to whatever is at
//the given index in the directory. Indexes start at 0!
static fs_node_t *initrd_readdir(fs_node_t *node, uint32_t index){

   //Check to make sure we did not 
   //go over the end.
   if( node->length <= index )
      return NULL; 

   fs_node_t *fsNode = (fs_node_t*)k_malloc( kernel_heap, sizeof(fs_node_t), 0);
   strcpy( fsNode->name, current_initrd.initrdObjects[index].name );
   fsNode->mask = fsNode->uid = fsNode->gid = fsNode->impl = 0;
   fsNode->flags = current_initrd.initrdObjects[index].flags;
   fsNode->inode = current_initrd.initrdObjects[index].inode;
   fsNode->length = current_initrd.initrdObjects[index].length;
   
   //We will need to setup the function pointers differently
   //depending on if this is a file or directory.
   if( fsNode->flags & FS_FILE ){
      //This is a file, the only operations that make
      //sense are read,write,close,and open. We currently
      //do not implement open,close as that is a far
      //higher abstraction that we need atm.
      fsNode->read = initrd_readfile;
      fsNode->write = initrd_writefile;
      fsNode->open = NULL;
      fsNode->close = initrd_close; 
      fsNode->readdir = NULL;
      fsNode->finddir = NULL;
   }
   return fsNode;
}

//This function searches the given node (MUST BE A DIRECTORY)
//for the given file/directory name (currently only supports files.
static fs_node_t *initrd_finddir(fs_node_t *node, char *name){

   fs_node_t *fsNode;

   for(int item = 0; item < current_initrd.numFiles; item++){
      //We have found the file!
      if( ! strcmp( name, current_initrd.initrdObjects[item].name) ){
         fsNode = (fs_node_t*)k_malloc(kernel_heap, sizeof(fs_node_t), 0);

         strcpy( fsNode->name, current_initrd.initrdObjects[item].name );
         fsNode->mask = fsNode->uid = fsNode->gid = fsNode->impl = 0;
         fsNode->flags = current_initrd.initrdObjects[item].flags;
         fsNode->inode = current_initrd.initrdObjects[item].inode;
         fsNode->length = current_initrd.initrdObjects[item].length;

         //The function pointers need to be setup differently
         //depending if we are dealing with a file or directory.
         if( fsNode->flags & FS_FILE ){
            fsNode->read = initrd_readfile;
            fsNode->write = initrd_writefile;
            fsNode->open = NULL;
            fsNode->close = initrd_close; 
            fsNode->readdir = NULL;
            fsNode->finddir = NULL;
         }
      }
   }
   return fsNode;
}


//This function will initilize the initrd for use.
//It will create a single fs_node_t for use by
//the caller. fs_node_t's representing each file/dir
//in the initrd will be created on an as-needed basis
//This is the only function that any outside code needs
//to know about.
fs_node_t *init_initrd(uint32_t *addr){
   //Verify the initrd magic number
   if( *addr != INITRD_MAGIC )
      return NULL;

   //TODO do more error checking?
   
   //Initilize the current_initrd strucute
   current_initrd.initrdHeader = (struct initrd_header*)addr;
   current_initrd.numFiles = current_initrd.initrdHeader->numFiles;

   //We add 2 since initrd_header consists of two uint32_t members.
   //If we had put sizeof( struct initrd_header ), it would have 
   //resolved to 8 (2 * uint32_t), at which point the compiler
   //would increment addr BY 8 * 4 = 32 BYTES, NOT THE 8 * 2 = 16 
   //THAT WE NEED
   current_initrd.initrdObjects = (struct initrd_object*)(addr + 2);

   fs_node_t *initrd = (fs_node_t*)k_malloc(kernel_heap, sizeof(fs_node_t), 0);
   strcpy( initrd->name, "initrd");
   initrd->mask = initrd->uid = initrd->gid = initrd->inode = initrd->impl = 0;
   initrd->flags = FS_DIRECTORY;
   initrd->length = current_initrd.initrdHeader->numFiles;

   //The initrd is treated as a directory, the only functions 
   //applicable are readdir and finddir
   initrd->read = NULL;
   initrd->write = NULL;
   initrd->open = NULL;
   initrd->close = initrd_close;
   initrd->readdir = initrd_readdir;
   initrd->finddir = initrd_finddir;
   initrd->ptr = NULL;

   return initrd;
}
