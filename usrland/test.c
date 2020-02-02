#include "stdlib.h"

void main(){

   char msg[] = "In userland\n";
   syscall_write(stdout, msg, sizeof(msg));
   int pid = syscall_getpid();

   char buf[10];
   syscall_read(stdin, buf, 10);

   syscall_write(stdout, buf, sizeof(buf));

   syscall_exit(0);

/*
   char msg[] = "Userland stdin test: \n";
   syscall_write(stdout, msg, sizeof(msg));

   char input[3];
   syscall_read(stdin, input, 2);
   input[2] = 0;
   char msg2[] = "Read in: ";
   syscall_write(stdout, msg2, sizeof(msg2));
   syscall_write(stdout, input, 2);

   syscall_read(stdin, input, 2);
   syscall_write(stdout, input, 2);
*/

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
}
