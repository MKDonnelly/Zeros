#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define INITRD_MAGIC 0x12345678
#define FILENAME_MAX_LEN 32
#define NUM_INITRD_FILES 32

//A header describing the initrd.
struct initrd_header{
   uint32_t magic;
   uint32_t numFiles;
};

#define INITRD_FILE 0x01
#define INITRD_DIR  0x02


//The file header describing a 
//file within the initrd.
struct file_header{
   char name[FILENAME_MAX_LEN];
   uint32_t inode;
   uint32_t offset;
   uint32_t length;
   uint32_t  flags; 
};

//./makeinitrd file1 file2 initrd
int main(char argc, char **argv){

   //Catch wrong number of files
   if( argc < 3 ){
      printf("ERROR: Wrong number of files\n");
      return -1;
   }

   //Write the initrd header
   FILE *initrd = fopen( argv[argc - 1], "w" );

   struct initrd_header head = {
          .magic = INITRD_MAGIC,
          .numFiles = argc - 2
   };
   fwrite( &head, sizeof(struct initrd_header), 1, initrd );

   //Create a default template for the fixed number
   //of file descriptors and write them to the initrd.
   //Later on, we come back and put valid data in these
   //to reflect valid files added.
   struct file_header template = {
          .name = 0,
          .inode = 0,
          .offset = 0,
          .length = 0, 
          .flags = 0
   };
  
   for(int i = 0; i < NUM_INITRD_FILES; i++){
      fwrite( &template, sizeof(struct file_header), 1, initrd ); 
   }
 
   //Calculate the current offset in the file
   int curOffset = sizeof(struct initrd_header) + 
                   NUM_INITRD_FILES * sizeof(struct file_header);


   //Now we will loop through each file passed to us on the 
   //command line, read it char-by-char into the initrd, calculate
   //its offset and length, and store that information into lengths 
   //and offsets.
   int lengths[NUM_INITRD_FILES];
   int offsets[NUM_INITRD_FILES];
   for(int i = 1; i <= argc - 2; i++){
      offsets[i-1] = curOffset;
      //Open the file to put in the initrd
      FILE *fileToPlace = fopen( argv[i], "r" );
      int fileSize = 0;
      char temp;
      while( (temp = getc( fileToPlace ) ) != EOF ){
         fputc( temp, initrd );
         fileSize++;
      }
      lengths[i-1] = fileSize;
      curOffset += fileSize;
      fclose( fileToPlace );
   }

   //We now go back and update the file_header structures
   //at the beginning of the file using lengts and offsets above.
   int headerOffset = sizeof( struct initrd_header );
   struct file_header temp;

   for(int i = 0; i < argc - 2; i++){
      strncpy( temp.name, argv[i+1], FILENAME_MAX_LEN);
      temp.offset = offsets[i];
      temp.length = lengths[i];
      temp.inode = i;
      temp.flags = INITRD_FILE;
      fseek( initrd, headerOffset, SEEK_SET );
      fwrite( &temp, sizeof(struct file_header), 1, initrd );
      headerOffset += sizeof(struct file_header);
   }

   fclose(initrd);

   return 0;
}
