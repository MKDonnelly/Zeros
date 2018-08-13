#include "stdlib.h"

void main(){
   char msg[] = "Userland stdin test: ";
   syscall_write(stdout, msg, sizeof(msg));

   char input[2];
   syscall_read(stdin, input, 2);
   char msg2[] = "Read in: ";
   syscall_write(stdout, msg2, sizeof(msg2));
   syscall_write(stdout, input, 2);


//   syscall_write(stdout, input, 2);
/*   char name[] = "data.txt";
   char msg[] = "Usrland: opening data.txt";
   syscall_write(stdout, msg, sizeof(msg));
   int fd = syscall_open(name);

   char buf[20];
   syscall_read(fd, buf, 10);

   char msg3[] = "Usrland read: ";
   syscall_write(stdout, msg3, sizeof(msg3));
   syscall_write(stdout, buf, 10);*/

   syscall_exit(0);   
}

