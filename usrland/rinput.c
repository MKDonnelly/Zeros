#include "stdlib.h"

//Takes input from stdin and writes
//to stdout
int main(){
   char buf[128];
   int ret;

   while( (ret = syscall_read(stdin, buf, 128)) != 0 ){
      syscall_write(stdout, buf, ret);
   }

   syscall_exit(0);
   return 0;
}
