#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

//TODO merge this with the in-kernel initrd code to have a single
//     version of this.
#define INITRD_MAGIC 0x12345678
#define FILENAME_MAX_LEN 32

//A superblock for the initrd
typedef struct initrd_sb{
   uint32_t magic;
   uint32_t entries_count;
}initrd_sb_t;

//Last bit in flags determines if this
//is a file or directoty.
#define INITRD_FILE 0x00
#define INITRD_DIR  0x01

//The initrd_inode describes an entity
//within an initrd.
typedef struct initrd_inode{
   char name[FILENAME_MAX_LEN];
   uint32_t inode;
   uint32_t offset;
   uint32_t length;
   uint32_t  flags; 
}initrd_inode_t;


//./makeinitrd file1 file2 initrd
//Args:
// Last argument is name of initrd output.
// All other arguments are for file to
// store within the initrd.
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

   //Create an initrd superblock
   initrd_sb_t superblock = {
     .magic = INITRD_MAGIC,
     .entries_count = total_files, 
   };
 
   //Write the superblock
   fwrite(&superblock, sizeof(superblock), 1, initrd);

   //Create a default template for the fixed number
   //of file descriptors and write them to the initrd.
   //Later on, we come back and put valid data in these
   //to reflect valid files added.
   initrd_inode_t template;
   memset(&template, sizeof(template), 0);
  
   for(int i = 0; i < total_files; i++){
      fwrite(&template, sizeof(template), 1, initrd);
   }
 
   //Now that we have wrote the headers, calculate where we
   //currently are in the file.
   int initrd_offset = sizeof(initrd_sb_t) + 
                           total_files * sizeof(initrd_inode_t);


   //Now we will loop through each file passed to us on the 
   //command line, read it char-by-char into the initrd, calculate
   //its offset and length, and store that information into lengths 
   //and offsets.
   int *lengths = malloc(sizeof(int) * total_files);
   int *offsets = malloc(sizeof(int) * total_files);

   for(int i = 0; i <= total_files; i++){
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
      char temp;
      while( (temp = getc(initrd_file)) != EOF )
         fputc(temp, initrd);
      fclose( initrd_file);
   }

   //We now go back and update the file_header structures
   //at the beginning of the file using lengts and offsets above.
   int headers_start = sizeof(initrd_sb_t);
   fseek(initrd, headers_start, SEEK_SET);
   initrd_inode_t temp;

   for(int i = 0; i < total_files; i++){
      strncpy(temp.name, argv[i+1], FILENAME_MAX_LEN);
      temp.offset = offsets[i];
      temp.length = lengths[i];
      temp.inode = i;
      temp.flags = INITRD_FILE;
      fwrite(&temp, sizeof(temp), 1, initrd );
   }

   fclose(initrd);

   return 0;
}
