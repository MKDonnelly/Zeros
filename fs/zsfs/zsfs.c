#include <fs/zsfs/zsfs.h>
#include <kernel/mm/heap.h>
#include <lib/string.h>
#include <lib/print.h>

//We commonly need to read/write blocks of data. Instead of 
//declaring a bunch of arrays of 512 bytes or using k_malloc,
//just use this one.
char zeros[512];

//This is like a class.  We create a copy of this for every
//ZSFS filesystem being used.
fstype_t zsfs_fs = {
   .fs_type = ZSFS_TYPE,
   .fs_id = 0, //we don't know until we instantiate the fs
   .parent = NULL, //again, we can't tell
   .check_type = zsfs_check, //used to check the fs type
   .get_id = zsfs_get_id, //get unique id. this goes in fs_id above
   .open_inodes = NULL, //array of open inodes
   .root_dir = {
      .name = "/",
      .flags = 0,
      .fs = NULL,
      .inode = 0,  //Inodes are assigned sequentially

      .read = zsfs_read,
      .write = zsfs_write,
      .open = zsfs_open,
      .close = zsfs_close,
      .readdir = zsfs_readdir,
      .finddir = zsfs_finddir,
   },
};

//Register the filesystem with the fsmanager
void zsfs_register(){
   fsmanager_register_fstype(&zsfs_fs);
}

int zsfs_check(char *buf){
   if( strncmp( buf, ZSFS_MAGIC, ZSFS_MAGIC_LEN) == 0 ){
      k_printf("[zsfs] Found filesystem on blkdev\n");
      return 1;
   }
   return 0;
}

void zsfs_create(blkdev_t *block){
   k_printf("Creating new zsfs filesystem\n");
   //The new superblock
   zsfs_sb_t new_sb;
   //Set magic field
   memcpy(new_sb.magic, ZSFS_MAGIC, ZSFS_MAGIC_LEN);

   //In the future, have this be a random value
   new_sb.fs_id = 0x1234ABCD;

   //Start the FBL at block index 1 (512 bytes into the drive)
   new_sb.fbl_index = 1;

   //Calculate how big the FBL needs to be.  Every 1 bit in the FBL
   //records one block in the Available Space segment.

   //Add a 1 to take care of any fractional block requirements
   new_sb.fbl_len = (((block->max_lba - block->min_lba) / block->block_size) + 1) * 512;

   //The root directory structure comes right after the FBL
   new_sb.rds_index = new_sb.fbl_index + (new_sb.fbl_len / 512);

   //Initialize the FBL with all zeros, and then set the blocks that 
   //the superblock, FBL, and RDE use as allocated.
   memset(zeros, block->block_size, 0);
   for(int i = new_sb.fbl_index; i < new_sb.fbl_index + new_sb.fbl_len / 512; i++){
      //The "1" is the number of blocks to write, which is 1 in this case.
      block->write_lba( block, i, 1, zeros );
   }

   //Set the first few bits for the superblock, FBL, and RDE
   for(int i = 0; i <= new_sb.rds_index; i++){
      bit_set(zeros, i);
   }
   block->write_lba(block, new_sb.fbl_index, 1, zeros);


   //Also, zero out the RDE since it starts empty
   memset(zeros, block->block_size, 0);
   block->write_lba(block, new_sb.rds_index, 1, zeros);

   //Write the new superblock.
   block->write_lba( block, 0, sizeof(zsfs_sb_t), (char*)&new_sb);
}

uint32_t zsfs_get_id(char *buf){
   zsfs_sb_t *sb = (zsfs_sb_t*)buf;
   return sb->fs_id;
}

///////////////////////////////////////////////////////////

//Read the rds_index field from the superblock
int get_rds(fstype_t *zsfs){
   zsfs->parent->read_lba(zsfs->parent, 0, 1, zeros);
   return ((zsfs_sb_t*)zeros)->rds_index;
}

//Returns fs_node_t of root
fs_node_t *zsfs_get_root(fstype_t *zsfs){
   fs_node_t *zsfs_root = k_malloc(sizeof(fs_node_t), 0);

   strcpy(zsfs_root->name, "/");
   zsfs_root->type = FS_DIRECTORY;
   zsfs_root->fs = zsfs;
   
   //Get inode for root
   zsfs_dinode_t *root_inode = k_malloc(sizeof(zsfs_dinode_t), 0);
   root_inode->blk_idx = get_rds(zsfs);
   zsfs_root->inode = root_inode;

   zsfs_root->readdir = zsfs_readdir;
   zsfs_root->finddir = zsfs_finddir;

   return zsfs_root;
}

//Reads the entry specified at index for the directory
fs_node_t *zsfs_finddir(fs_node_t *zsfs_dir, char *name){
   fstype_t *zsfs = zsfs_dir->fs;
   //Read in DS for the directory
   zsfs_ds_t ds_search;
   int ds_blk_idx = ((zsfs_dinode_t*)zsfs_dir->inode)->blk_idx;
   zsfs->parent->read_lba(zsfs->parent, ((zsfs_dinode_t*)zsfs_dir->inode)->blk_idx, 1, (char*)&ds_search);

   //Loop through the DS and all of the linked DSes until the entry is found
   //or we have reached the end.
   while( 1 ){
      //Search through the current DS
      for(int ds_entry = 0; ds_entry < ZSFS_DSE_IN_DS; ds_entry++){
         //Found the entry.  Now return a fs_node_t to it.
         if( strcmp(ds_search.entries[ds_entry].entry_name, name) == 0 ){
            fs_node_t *new_node = k_malloc(sizeof(fs_node_t), 0);
            
            //Check if this is a directory or a file
            if( ds_search.entries[ds_entry].type == 0 ){ //0 == file
               strcpy(new_node->name, name);
               new_node->type = FS_FILE;
               new_node->fs = zsfs_dir->fs;

               //Create inode for this file.
               zsfs_finode_t *finode = k_malloc(sizeof(zsfs_finode_t), 0);
               finode->ds_blk_idx = ds_blk_idx;
               finode->ds_entry_idx = ds_entry;

               new_node->inode = finode;

               new_node->read = zsfs_read;
               new_node->write = zsfs_write;
               new_node->open = zsfs_open;
               new_node->close = zsfs_close;
               new_node->len = zsfs_len;
            }else{ //1 == directory
               strcpy(new_node->name, name);
               new_node->type = FS_DIRECTORY;
               new_node->fs = zsfs_dir->fs;

               //Create inode for this directory.
               zsfs_dinode_t *dinode = k_malloc(sizeof(zsfs_dinode_t), 0);
               dinode->blk_idx = ds_search.entries[ds_entry].blk_idx;

               new_node->inode = dinode;

               new_node->readdir = zsfs_readdir;
               new_node->finddir = zsfs_finddir;
            }

            return new_node;
         }
      }

      //If we get here, then the last DS did not contain the entry. 
      //Check to see if there is a linked directory using the AUX field
      if( ds_search.aux_entry.linked_dir != 0 ){
         //Set the blk idx for this next DS
         ds_blk_idx = ds_search.aux_entry.linked_dir;

         //Read in the next DS
         zsfs->parent->read_lba(zsfs->parent, ds_blk_idx, 1, (char*)&ds_search);
          
      }else{
         //Object not found
         return 0;
      }
   }
}


dirent_t *zsfs_readdir(fs_node_t *dir, int index){
   fstype_t *zsfs = dir->fs;
   //Read in DS for the directory
   zsfs_ds_t ds_search;
   int ds_blk_idx = ((zsfs_dinode_t*)dir->inode)->blk_idx;
   zsfs->parent->read_lba(zsfs->parent, ((zsfs_dinode_t*)dir->inode)->blk_idx, 1, (char*)&ds_search);

   //Loop through the DS and all of the linked DSes until the entry is found
   //or we have reached the end.
   while( index >= 0 ){
      //The DSE at index is not on this DS, so read in the next one.
      if( index > ZSFS_DSE_IN_DS ){
         index -= ZSFS_DSE_IN_DS;

         if( ds_search.aux_entry.linked_dir != 0 ){
            //Set the blk idx for this next DS
            ds_blk_idx = ds_search.aux_entry.linked_dir;

            //Read in the next DS
            zsfs->parent->read_lba(zsfs->parent, ds_blk_idx, 1, (char*)&ds_search);         }else{
            //Entry past end of directory
            return NULL;
         }
      }else{ //index is on the current block
         if( ds_search.entries[index].blk_idx == 0 &&
             ds_search.entries[index].entry_len == 0 ){
            return NULL;
         }else{
            dirent_t *new_de = k_malloc(sizeof(dirent_t), 0);
            strcpy(new_de->name, ds_search.entries[index].entry_name);
            return new_de;
         }  
      }
   }
}


//Returns the inode of the object at "path" if it exists, NULL otherwise.
fs_node_t *zsfs_open(fs_node_t *root, char *path){

   //Parse each path component
   //Make a copy of path since we modify it
   char *newpath = k_malloc(strlen(path), 0);
   strcpy(newpath, path);

   //Parse each component of the path
   int ccount = 0;
   for(int i = 0; i < strlen(newpath); i++){
      //Number of '/' should equal the number of components
      //in the path if we leave off an optional trailing '/'
      if( newpath[i] == '/' && i+1 >= strlen(newpath) ){
         ccount++;
      }
   }

   int cnum = 0;
   char **path_components;
   path_components = k_malloc(sizeof(char*) * ccount, 0);
   int newpath_len = strlen(newpath);
   for(int i = 0; i < newpath_len; i++){
      if( newpath[i] == '/' ){
         newpath[i] = '\0';
         if( newpath[i+1] != '\0' ){
            path_components[cnum] = &newpath[i+1];
            cnum++;
         }
      }
   }

   //Loop through every component
/*   root = get_root(zsfs);
   for(int i = 0; i < cnum; i++){
      //Search for the component in root and place its inode in root (overwrite)
      int ret = dir_search(root, root, path_components[i], zsfs);
      if( ret == 0 ){
         k_printf("Couldn't find component %s\n", path_components[i]);
         return NULL;
      }
   }*/
   return NULL;
}


void zsfs_close(fs_node_t *fsnode){
   //Free inode
   k_free(fsnode->inode);

   //Free fsnode
   k_free(fsnode);

   //Caller should set pointer to null after.
}

int zsfs_len(fs_node_t *file){
   if( file->type == FS_FILE ){
      //Read in the DS
      zsfs_ds_t ds;
      file->fs->parent->read_lba(file->fs->parent, ((zsfs_finode_t*)file->inode)->ds_blk_idx, 1, (char*)&ds);

      return ds.entries[((zsfs_finode_t*)file->inode)->ds_entry_idx].entry_len;
   }else{
      return -1;
   }
}


int get_fbl_entry(int index, fstype_t *zsfs){
   //Read in the superblock
   zsfs->parent->read_lba(zsfs->parent, 0, 1, zeros);
   zsfs_sb_t *sb = &zeros;

   //Using the superblock, read in the correct FBL block
   int fbl_block = sb->fbl_index + (index / 512);
   
   //Read in the FBL block
   zsfs->parent->read_lba(zsfs->parent, fbl_block, 1, zeros);

   //Get bit at the correct index
   char val = bit_get( (void*)zeros, index % 512 );

   return val;
}

void set_fbl_entry(int index, fstype_t *zsfs){
   //Read in the superblock
   zsfs->parent->read_lba(zsfs->parent, 0, 1, zeros);
   zsfs_sb_t *sb = &zeros;

   //Using the superblock, read in the correct FBL block
   int fbl_block = sb->fbl_index + (index / 512);
   
   //Read in the FBL block
   zsfs->parent->read_lba(zsfs->parent, fbl_block, 1, zeros);

   //Set the correct bit
   bit_set( (void*)zeros, index % 512 );

   //Write the block back
   zsfs->parent->write_lba(zsfs->parent, fbl_block, 1, zeros);
}


void clear_fbl_entry(int index, fstype_t *zsfs){
   //Read in the superblock
   zsfs->parent->read_lba(zsfs->parent, 0, 1, zeros);
   zsfs_sb_t *sb = &zeros;

   //Using the superblock, read in the correct FBL block
   int fbl_block = sb->fbl_index + (index / 512);
   
   //Read in the FBL block
   zsfs->parent->read_lba(zsfs->parent, fbl_block, 1, zeros);

   //Set the correct bit
   bit_clear( (void*)zeros, index % 512 );

   //Write the block back
   zsfs->parent->write_lba(zsfs->parent, fbl_block, 1, zeros);
}


int allocate_block(fstype_t *zsfs){
   char temp[512];

   //Read in the superblock
   zsfs->parent->read_lba(zsfs->parent, 0, 1, temp);
   zsfs_sb_t *sb = (zsfs_sb_t*)temp;

   //Look for a free bit
   int idx = 0;
   for(int i = 0; i < sb->fbl_len; i++){
      if(get_fbl_entry(i, zsfs) == 0 ){
         set_fbl_entry(i, zsfs);
         return i;
      }
   }
   return -1;
}


//Creates a new directory under parent_dir with the given name. 
void zsfs_create_dir(fs_node_t *parent_dir, char *name){
   //Read in the parent directory DS
   zsfs_ds_t ds;
   fstype_t *zsfs = parent_dir->fs;
   int ds_idx = ((zsfs_dinode_t*)parent_dir->inode)->blk_idx;
   zsfs->parent->read_lba(zsfs->parent, ds_idx, 1, (char*)&ds);

   //Find a free location in the DS to add a new DSE for the new directory
   while( 1 ){

      //Scan through the current directory for a free spot
      for(int dse = 0; dse < 7; dse++){
         //Found a free DSE location.  Add new directory
         if( ds.entries[dse].blk_idx == 0 && ds.entries[dse].entry_len == 0 ){
            //Allocate new block for the subdir and zero it out
            int new_subdir_blk_idx = allocate_block(zsfs);

            if( new_subdir_blk_idx == -1 ){
               k_printf("Failed to allocate block\n");
               return -1;
            }
 
            memset(zeros, 512, 0);
            zsfs->parent->write_lba(zsfs->parent, new_subdir_blk_idx, 1, zeros);

            //Add the entry in this DS
            ds.entries[dse].type = 1; //1 == directory
            ds.entries[dse].blk_idx = new_subdir_blk_idx;
            ds.entries[dse].entry_len = 0; //Doesn't matter for directories
            strcpy(ds.entries[dse].entry_name, name);

            //Finally, write the modified DS back to the drive
            zsfs->parent->write_lba(zsfs->parent, ds_idx, 1, (char*)&ds);
            return;
         }
      }

      //Scanned through this DS and no free spot found.  No linked dir
      //either, so create a new linked dir.
      if( ds.aux_entry.linked_dir == 0 ){
         //Allocate a new block for the new linked directory
         int linked_dir_idx = allocate_block(zsfs);

         if( linked_dir_idx == -1 ){
            k_printf("Failed to allocate block\n");
            return -1;
         }

         //Link up the linked directory
         ds.aux_entry.linked_dir = linked_dir_idx;

         //Write out the parent DS to finalize linking
         zsfs->parent->write_lba(zsfs->parent, ds_idx, 1, (char*)&ds);

         //Make the DS for the linked directory and add the new subdir
         zsfs_ds_t linked_dir;
         //Zero it out
         memset((void*)&linked_dir, sizeof(zsfs_ds_t), 0);

         //Because the linked directory was just created, it must be empty.
         //Therefore, the new DSE can be directly added
         int new_subdir_blk = allocate_block(zsfs);
         zsfs_ds_t new_subdir;
         memset((void*)&new_subdir, sizeof(zsfs_ds_t), 0);
         zsfs->parent->write_lba(zsfs->parent, new_subdir_blk, 1, (char*)&new_subdir);

         //Add the subdirectory to the linked directory
         linked_dir.entries[0].type = 1; //1 == directory
         linked_dir.entries[0].blk_idx = new_subdir_blk;
         strcpy(linked_dir.entries[0].entry_name, name);
         
         //Write out the linked directory
         zsfs->parent->write_lba(zsfs->parent, linked_dir_idx, 1, (char*)&linked_dir);

      //Scanned through this DS and no free spot found.  But there is a
      //linked DS, so read it in and scan through it on the next iter.
      }else if( ds.aux_entry.linked_dir != 0 ){
         //Set the linked directory as the ds we are looking at
         ds_idx = ds.aux_entry.linked_dir;
         
         //Read in the new DS
         zsfs->parent->read_lba(zsfs->parent, ds_idx, 1, (char*)&ds);
         //On the next loop, we will search through this DS
      }
   }
}

//Creates a new directory under parent_dir with the given name. 
void create_file(fs_node_t *parent_dir, char *name){
   fstype_t *zsfs = parent_dir->fs;
   //Read in the parent directory DS
   zsfs_ds_t ds;
   int ds_idx = ((zsfs_dinode_t*)parent_dir->inode)->blk_idx;
   zsfs->parent->read_lba(zsfs->parent, ds_idx, 1, (char*)&ds);
   
   //Find a free location in the DS to add a new DSE for the new file
   while( 1 ){

      //Scan through the current directory for a free spot
      for(int dse = 0; dse < 7; dse++){
         //Found a free DSE location.  Add new file
         if( ds.entries[dse].blk_idx == 0 && ds.entries[dse].entry_len == 0 ){
            //Allocate new block for the file and zero it out
            int new_file_blk_idx = allocate_block(zsfs); 

            if( new_file_blk_idx == -1 ){
               k_printf("Failed to allocate block\n");
               return;
            }

            memset(zeros, 512, 0);
            zsfs->parent->write_lba(zsfs->parent, new_file_blk_idx, 1, zeros);

            //Add the entry in the DS
            ds.entries[dse].type = 0; //0 == file
            ds.entries[dse].blk_idx = new_file_blk_idx;
            ds.entries[dse].entry_len = 0; 
            strcpy(ds.entries[dse].entry_name, name);

            //Finally, write the modified DS back to the drive
            zsfs->parent->write_lba(zsfs->parent, ds_idx, 1, (char*)&ds);
            return;
         }
      }

      //Scanned through this DS and no free spot found.  No linked dir
      //either, so create a new linked dir.
      if( ds.aux_entry.linked_dir == 0 ){
         //Allocate a new block for the new linked directory
         int linked_dir_idx = allocate_block(zsfs);

         //Link up the linked directory
         ds.aux_entry.linked_dir = linked_dir_idx;

         //Write out the parent DS to finalize linking
         zsfs->parent->write_lba(zsfs->parent, ds_idx, 1, (char*)&ds);

         //Make the DS for the linked directory and add the new file
         zsfs_ds_t linked_dir;
         //Zero it out
         memset((void*)&linked_dir, sizeof(zsfs_ds_t), 0);

         //Because the linked directory was just created, it must be empty.
         //Therefore, the new DSE can be directly added
         int new_file_blk = allocate_block(zsfs);
         char new_file[512];
         memset((void*)new_file, 512, 0);
         zsfs->parent->write_lba(zsfs->parent, new_file_blk, 1, (char*)new_file);

         //Add the file to the linked directory
         linked_dir.entries[0].type = 0; //0 == file
         linked_dir.entries[0].blk_idx = new_file_blk;
         strcpy(linked_dir.entries[0].entry_name, name);
         
         //Write out the linked directory
         zsfs->parent->write_lba(zsfs->parent, linked_dir_idx, 1, (char*)&linked_dir);

      //Scanned through this DS and no free spot found.  But there is a
      //linked DS, so read it in and scan through it on the next iter.
      }else if( ds.aux_entry.linked_dir != 0 ){
         //Set the linked directory as the ds we are looking at
         ds_idx = ds.aux_entry.linked_dir;
         
         //Read in the new DS
         zsfs->parent->read_lba(zsfs->parent, ds_idx, 1, (char*)&ds);
         //On the next loop, we will search through this DS
      }
   }
}



int zsfs_read(fs_node_t *file, int offset, int len, char *buf){
   //Read in the DS that holds the file DSE
   int ds_blk_idx = ((zsfs_finode_t*)file->inode)->ds_blk_idx;
   int ds_entry_idx = ((zsfs_finode_t*)file->inode)->ds_entry_idx;

   zsfs_ds_t ds;
   file->fs->parent->read_lba(file->fs->parent, ds_blk_idx, 1, (char*)&ds);

   //Get reference to DSE for this file
   zsfs_dse_t *dse = &ds.entries[ds_entry_idx];

   zsfs_file_t f;

   int file_blk_idx = dse->blk_idx;
   file->fs->parent->read_lba(file->fs->parent, dse->blk_idx, 1, (char*)&f);

   //Loop over each block until we are at the block where the first byte
   //is read from.
   while( offset > 508 ){
      offset -= 508;
      //Read in next block of the file
      file_blk_idx = f.next_blk;
      file->fs->parent->read_lba(file->fs->parent, f.next_blk, 1, (char*)&f);
   }

   //Now we know the block in "file" contains at least the first index to write.
   int file_idx = offset;
   int buf_idx = 0;
   while( len > 0 ){

      buf[buf_idx++] = f.data[file_idx++];
      len--;

      //Need to read in next block of file if there is one.
      if( file_idx == 508 ){
         file_idx = 0;

         //Next block exists, read it in
         if( f.next_blk != 0 ){
            file_blk_idx = f.next_blk;
            file->fs->parent->read_lba(file->fs->parent, f.next_blk, 1, (char*)&f);
         }else{ //next block does not exist.  Return bytes not read yet.
            return len;  
         }
      }
   }

   return len;
}


int zsfs_write(fs_node_t *file, int offset, int len, char *buf){
   //Read in the DS that holds the file DSE
   int ds_blk_idx = ((zsfs_finode_t*)file->inode)->ds_blk_idx;
   int ds_entry_idx = ((zsfs_finode_t*)file->inode)->ds_entry_idx;

   zsfs_ds_t ds;
   file->fs->parent->read_lba(file->fs->parent, ds_blk_idx, 1, (char*)&ds);

   //Get reference to DSE for this file
   zsfs_dse_t *dse = &ds.entries[ds_entry_idx];

   zsfs_file_t f;

   int file_blk_idx = dse->blk_idx;
   file->fs->parent->read_lba(file->fs->parent, dse->blk_idx, 1, (char*)&f);

   //Loop over each block until we are at the block where the first byte
   //is written to.
   while( offset > 508 ){
      offset -= 508;
      //Read in next block of the file
      file_blk_idx = f.next_blk;
      file->fs->parent->read_lba(file->fs->parent, f.next_blk, 1, (char*)&f);
   }

   //Now we know the block in "file" contains at least the first index to write.
   int file_idx = offset;
   int buf_idx = 0;
   while( len > 0 ){

      f.data[file_idx++] = buf[buf_idx++];
      len--;

      //Need to read in next block of file if there is one.
      if( file_idx == 508 ){
         file_idx = 0;
         //Write out this current block
         file->fs->parent->write_lba(file->fs->parent, file_blk_idx, 1, (char*)&f);

         //Next block exists, read it in
         if( f.next_blk != 0 ){
            file_blk_idx = f.next_blk;
            file->fs->parent->read_lba(file->fs->parent, f.next_blk, 1, (char*)&f);
         }else{ //next block does not exist.  Create it
            file_blk_idx = allocate_block(file->fs);  
         }
      }
   }
   //Finally, write out the last block
   file->fs->parent->write_lba(file->fs->parent, file_blk_idx, 1, (char*)&f);

   //Update file length if needed
   if( file_idx > dse->entry_len ){
      dse->entry_len = file_idx;
      //Write out updates DSE
      file->fs->parent->write_lba(file->fs->parent, ds_blk_idx, 1, (char*)&ds);
   }

   return len;
}
