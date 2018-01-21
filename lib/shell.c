#include <shell.h>

char input_buffer[128];
int buffer_len;

void shell_start(){

   k_printf("User@Ring0-> ");

   char input;
   buffer_len = 0;

   while( 1 ){    
      if( input = getc() ){

         if( input == '\r' ){
            input_buffer[buffer_len] = 0;

            k_printf("Return Entered! ");
            if( strcmp(input_buffer, "hello") ){
                k_printf("Hello back");
            }else{
               k_printf("%s\n", input_buffer);
            }

            for(int i = 0; i < buffer_len; i++){
               input_buffer[i] = 0;
            }
            buffer_len = 0;
         }else{
            input_buffer[buffer_len] = input;
            buffer_len += 1;
         }
      }
   }
}
