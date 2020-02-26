#include "stdlib.h"

int main(){

   char args[128];
   syscall_getargs(args, 128);

   //Find character to search for
   char *search_for = args;
   while( search_for[0] == ' ' ) search_for++;

   //Find character to replace it with
   char *replace_with = search_for;
   replace_with++;
   while( replace_with[0] == ' ' ) replace_with++;

   //Read in input and make substitution as needed
   char buf[1024];
   int b_read = 0;
   while( (b_read = syscall_read(stdin, buf, 1024)) != 0 ){
      
      for(int i = 0; i < b_read; i++){
         if( buf[i] == search_for[0] ){
            buf[i] = replace_with[0];
         }
      }

      syscall_write(stdout, buf, b_read);
   }
  
   syscall_exit(0);
   return 0;
}   
