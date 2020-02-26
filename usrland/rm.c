#include "stdlib.h"

int main(){
   //Argument is name of file to remove
   char args[128];
   syscall_getargs(args, 128);

   //Delete object
   syscall_delete(args);

   syscall_exit(0);
   return 0;
}
