#include "stdlib.h"

int main(){
   //Takes a single argument
   //The name of the directory to create.
   //Directory is automatically created in the pwd
   char args[128];
   syscall_getargs(args, 128);

   //Get pwd where the directory will be created
   char pwd[128];
   syscall_getpwd(pwd, 128);

   syscall_created(pwd, args);
   syscall_exit(0);
   return 0;
}
