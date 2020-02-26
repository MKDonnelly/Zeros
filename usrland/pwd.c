#include "stdlib.h"

int main(){
   char pwd[100];
   syscall_getpwd(pwd, 100);
   syscall_write(stdout, pwd, mystrlen(pwd));
   newline();
   syscall_exit(0);
}
