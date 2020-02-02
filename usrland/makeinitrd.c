#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define INITRD_MAGIC 0x12345678
#define NAME_MAX_LEN 24

typedef struct initrd_sb{
   uint32_t magic; //Must be INITRD_MAGIC
   uint32_t entries_offset;
   uint32_t entries_len;
}initrd_sb_t;

typedef struct initrd_entry{
   uint32_t entry_offset;
   uint32_t entry_len;
   char name[NAME_MAX_LEN];
}initrd_entry_t;

int file_size(const char *file_name){
   struct stat s;

   if( stat(file_name, &s) == 0 ){
      return s.st_size;
   }else{
      return -1;
   }
}

int main(char argc, char **argv){

   //Catch wrong number of arguments
   if( argc < 3 ){
      printf("Format: ./makeinitrd <initrd out> <file1> <file2> ...\n");
      return -1;
   }

   //Open the first argument on the command line.  This will be
   //the initrd to write to.
   FILE *initrd = fopen(argv[1], "w");

   //Total number of files being written to the initrd.
   //subtract 2 since one of the arguments is "./makeinitrd" and
   //the other is <initrd out>. 
   int total_files = argc - 2;

   //Create the superblock
   initrd_sb_t initrd_sb;
   initrd_sb.magic = INITRD_MAGIC;
   initrd_sb.entries_offset = sizeof(initrd_sb); //entries start just after sb
   initrd_sb.entries_len = total_files;

   //Write the superblock to the output initrd
   fwrite(&initrd_sb, sizeof(initrd_sb), 1, initrd);
 
   //Setup each entry in the initrd
   initrd_entry_t entries[total_files];

   //Files start after the sb and entries table
   int offset = sizeof(initrd_sb) + sizeof(entries);
   for(int i = 0; i < total_files; i++){
      entries[i].entry_offset = offset;
      entries[i].entry_len = file_size(argv[i+2]);
      strcpy(entries[i].name, argv[i+2]);
   }

   //Write entries to initrd
   fwrite(&entries, sizeof(initrd_entry_t), total_files, initrd);

   //Now write the files to the initrd.  Start with the second
   //CLI argument.
   FILE *file_to_copy;
   for(int i = 0; i < total_files; i++){
      file_to_copy = fopen(argv[i+2], "r");
      int temp;
      while( (temp = getc(file_to_copy)) != EOF ){
         fputc(temp, initrd);
      }
      fclose(file_to_copy);
   }

   fclose(initrd);

   return 0;
}
