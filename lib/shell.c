#include <shell.h>

#define BUFFER_MAX_LEN 128

char input_buffer[BUFFER_MAX_LEN+1];
int buffer_len;

void zeros_shell(){

   char input;
   buffer_len = 0;

   k_printf( PROMPT );

   //Forever loop
   while( 1 ){    

      //Grab a character from the keyboard
      //buffer and put it into our buffer
      if( (input = getc()) ){

         //Return pressed, process command
         if( input == '\r' ){

             parse_command(input_buffer);

             //Zero out the buffer
             memset(input_buffer, BUFFER_MAX_LEN, 0); 

             //Reset our length
             buffer_len = 0;

             //Re-print our prompt
             k_printf( PROMPT );

        }else if( buffer_len + 1 < BUFFER_MAX_LEN ){
            //Key entered, put it in our buffer if
            //we have enough space
            input_buffer[buffer_len++] = input;
         }

      }else{
         //Yield so as to not burn up our cpu cycles
         thread_yield();
      }
   }
}

void parse_command(char *input_cmd){
   if( strcmp( input_cmd, "help" ) == 0){
      help();
   }else if( strcmp( input_cmd, "ls_files" ) == 0 ){
      int i = 0;
      fs_node_t *file;
      while( (file = fs_root->readdir( fs_root, i++ ))) {
         k_printf("%s\n", file->name);
         file->close( file );
      }
   }//else if ( strcmp( input_cmd, "cat_file" ) == 0 ){
}

void help(){
   k_printf("Zeros shell help:\n");
   k_printf("   help - Print this menu\n");
   k_printf("   ls_files - List files in initrd\n");
}
