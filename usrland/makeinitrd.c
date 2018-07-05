#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define INITRD_MAGIC 0x12345678
#define FILENAME_MAX_LEN 32


#define INITRD_FILE 0
#define INITRD_DIR  1

//The initrd_inode describes an entity
//within an initrd. Note that a name is not stored here:
//the whole idea of having a directory is to associate 
//a name with an inode. Later on, this will be useful
//if links are supported.

//For a file, this describes it byte offset from the start of the
//initrd as well as its length in bytes. For a directory, this describes
//the byte offset of it contents (basicall, a name->inode# table), as well
//as the length AS THE NUMBER OF ENTRIES within the directory.
typedef struct initrd_inode{
   //My OS is 32 bit, so the two pointers here *should*
   //be 8 bytes total. However, the system compiler is 
   //64 bits, so this will end up being 16 bytes.
   //Just reserve 8 bytes here for the linked list node.
   char RESERVED[8];
   uint32_t inode;
   uint32_t offset;
   uint32_t length;
   uint32_t type; //currently, only directory or file
}initrd_inode_t;

//This is what a directory entry points to. The number of these
//structures is determined by the length
typedef struct initrd_dirent{
   char name[32];
   uint32_t inode;
}initrd_dirent_t;

//The initrd superblock. Just a regular inode describing the root,
//as well as a magic number to ensure that is is an initrd.
typedef struct initrd_sb{
   uint32_t magic;
   initrd_inode_t root_inode;
   //All inodes (except for root_inode) are kept in their own
   //contigeous section. This is the byte offset from the start
   //of the initrd.
   uint32_t inodes_offset; 
   uint32_t inodes_len;
}initrd_sb_t;


int main(char argc, char **argv){

   //Catch wrong number of files
   if( argc < 3 ){
      printf("ERROR: Wrong number of files\n");
      return -1;
   }

   //Open the last argument on the command line.
   //This is the output initrd.
   FILE *initrd = fopen(argv[argc-1], "w");

   //Total number of files to place in the initrd. Subtact 2
   //since one is "./makeinitrd" and the other is the output 
   //initrd name
   int total_files = argc - 2;

   //Write the superblock
   initrd_sb_t initrd_sb = {
      .magic = INITRD_MAGIC,
      .root_inode = {
         .inode = 0,
         .offset = sizeof(initrd_sb_t)+ sizeof(initrd_inode_t)*total_files, 
         .length = total_files, //length here is the number of entries
         .type = INITRD_DIR,
      }, 
      .inodes_offset = sizeof(initrd_sb), //inodes begin after sb
      //1 inode for each file
      .inodes_len = total_files,
   };

   fwrite(&initrd_sb, sizeof(initrd_sb), 1, initrd);
 
   //At this point, create temporary inodes for each file. These
   //are updated later
   initrd_inode_t template;
   fwrite(&template, sizeof(initrd_inode_t), total_files, initrd);

   //Now create the root directory contents, which will associate
   //a name to an inode
   for(int i = 0; i < total_files; i++){
      initrd_dirent_t new_file;
      strcpy(new_file.name, argv[i+1]);
      new_file.inode = i + 1;
      fwrite(&new_file, sizeof(new_file), 1, initrd);
   }


   //Now that we have wrote the inodes, calculate where we
   //currently are in the file.
   int initrd_offset = ftell(initrd); 

   //Now we will loop through each file passed to us on the 
   //command line, read it char-by-char into the initrd, calculate
   //its offset and length, and store that information into lengths 
   //and offsets.
   int *lengths = malloc(sizeof(int) * total_files);
   int *offsets = malloc(sizeof(int) * total_files);

   for(int i = 0; i < total_files; i++){
      //Open the file to put in the initrd. Add 1 since index 0
      //is the name of this program.
      FILE *initrd_file = fopen(argv[i+1], "r");

      offsets[i] = initrd_offset;      
      //we can calculate the length of the file by feeking to the end
      fseek(initrd_file, 0, SEEK_END);
      lengths[i] = ftell(initrd_file);
      fseek(initrd_file, 0, SEEK_SET);
      initrd_offset += lengths[i];

      //Write the file to the iniitrd 
      int temp;
      int i = 0;
      while( (temp = getc(initrd_file)) != EOF ){
         fputc(temp, initrd);
         i++;
      }
      fflush(initrd_file);
      fclose(initrd_file);
   }

   //We now go back and update the file_header structures
   //at the beginning of the file using lengts and offsets above.
   int inodes_start = sizeof(initrd_sb_t);
   fseek(initrd, inodes_start, SEEK_SET);
   initrd_inode_t temp;

   for(int i = 0; i < total_files; i++){
      temp.offset = offsets[i];
      temp.length = lengths[i];
      temp.inode = i + 1; //0 is reserved for the root inode. start @ 1
      temp.type = INITRD_FILE;
      fwrite(&temp, sizeof(temp), 1, initrd );
   }

   fclose(initrd);

   return 0;
}
