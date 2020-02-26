#include "stdlib.h"

int main(){
   //Accepts a single argument: 
   //  The absolute path to a file to
   //  write to stdout
   char fname[128];
   syscall_getargs(fname, 128);

   char file_path[128];
   mymemset(file_path, 128, 0);
   makeapath(fname, file_path);
   //make sure there are no trailing spaces
   char *t = file_path;
   while( t[0] != ' ' && t[0] != '\0') t++;
   t[0] = '\0';

   //Open file
   int fd = syscall_open(file_path);

   if( fd == -1 ){
      syscall_exit(1);
   }

   //Loop over file 128 bytes at a time
   char buf[128];
   int ret;
   while( (ret = syscall_read(fd, buf, 128)) != 0 ){
      //Write to stdout
      syscall_write(stdout, buf, ret);
   }

   syscall_exit(0);
   return 0;
}
