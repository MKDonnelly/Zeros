#include "stdlib.h"

int main(){
   //Accepts one argument
   //  The name of the file to create
   //The file is created in the pwd 

   char new_file[128];
   syscall_getargs(new_file, 128);

   char pwd[128];
   syscall_getpwd(pwd, 128);

   syscall_createf(pwd, new_file);
   syscall_exit(0);
   return 0;
}
