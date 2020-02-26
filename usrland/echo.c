#include "stdlib.h"

int mystrlen(char[]);

int main(){
   char args[128];
   int len = syscall_getargs(args, 128);
   //Put newline at end
   args[len] = '\n';
   args[len+1] = '\0';

   syscall_write(stdout, args, mystrlen(args));

   syscall_exit(0);
   return 0;
}
