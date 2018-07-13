#include "stdlib.h"

void main(){
   char name[] = "data.txt";
   char msg[] = "Usrland: opening data.txt";
   syscall_write(0, msg, sizeof(msg));
   syscall_open(name);

   syscall_exit(0);   
}

