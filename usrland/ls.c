#include "stdlib.h"

int main(){
   //This should have been spawned off by another
   //process.  We have inherited the pwd of the
   //parent, so request the pwd and then get a listing
   //of all the entries and their types.

   //Open directory and read each component
   char dir[100];
   syscall_getpwd(dir, 100);

   int fd = syscall_open(dir); 

   if( fd == -1 ){
      char m[] = "Error: could not open directory: ";
      syscall_write(stdout, m, mystrlen(m));
      syscall_write(stdout, dir, mystrlen(dir));
      newline();
   }

   char temp[100];
   int i = 0;

   while( syscall_readdir(fd, temp, 100, i) != -1 ){
      syscall_write(stdout, temp, mystrlen(temp));
      newline();
      i++;
      mymemset(temp, 100, 0);
   }   

   syscall_exit(0);
   return 0;
}

