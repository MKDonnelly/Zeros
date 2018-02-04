#include <lib/shell.h>

#define BUFFER_MAX_LEN 128

char input_buffer[BUFFER_MAX_LEN+1];
int buffer_len;

void zeros_shell(){

   //Horrible hack to start the
   //shell once the bootdemo is done
   while( startShell == 0 )
      thread_yield();
   
   char input;
   buffer_len = 0;

   k_printf( PROMPT );

   //Forever loop
   while( 1 ){    

      //Grab a linecharacter from the keyboard
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

        }else if( input == '\b'){
            input_buffer[--buffer_len] = 0;
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

   //When passing an argument, it is impossible to compare
   //the raw command name to the received buffer ("cmd" will
   //never equal "cmd arg1"). But we can tell if the first part
   //of the received string is the same as the reference string
   //by seeing if the first different char is ' '
   if( strcmp( input_cmd, "help" ) == 0){
      help();
   }else if( strcmp( input_cmd, "sysuptime" ) == ' ' ) {
      sysuptime( input_cmd[10] );
   }else if( strcmp( input_cmd, "ls_files" ) == 0 ){
      ls_files();
   }else if ( strcmp( input_cmd, "cat_file" ) == ' ' ){
      char *filename = &(input_cmd[9]);
      cat_file( filename );
   }else if( strcmp( input_cmd, "open_file") == ' ' ){
      //open_file( &input_cmd[10] );
      k_printf("Not working right now\n");
   }else if( strcmp( input_cmd, "clear") == 0 ){
      clear_screen();
   }else if( strcmp( input_cmd, "sysinfo" ) == 0 ){
      sysinfo();
   }else if( strcmp( input_cmd, "write_file" ) == ' ' ){

      char *args_start = &input_cmd[11];

      char filename_buf[100];
      char string_buf[100];

      //Copy over filename
      int i = 0;
      while( args_start[i] != ' ' ){
         filename_buf[i] = args_start[i];
         i++;
      }
      filename_buf[i] = 0;

      //Copy over string
      memcpy( string_buf, &args_start[i], buffer_len - i );

      write_file( filename_buf, string_buf );
   }
}

void help(){
   k_printf("Zeros shell help:\n");
   k_printf("   help - Print this menu\n");
   k_printf("   sysuptime <0|1> - Toggle system uptime w/ 1 or 0\n");
   k_printf("   ls_files - List files in initrd\n");
   k_printf("   cat_file <filename> - Dump the given file to the screen\n");
   k_printf("   open_file <filename> - Open file in pager\n");
   k_printf("   write_file <filename> <str> - Write <str> to <filename>\n");
   k_printf("   clear - Clear screen\n");
   k_printf("   sysinfo - Print system information\n");
}

void ls_files(){
      int i = 0;
      fs_node_t *file;
      while( (file = fs_root->readdir( fs_root, i++ ))) {
         k_printf("%s\n", file->name);
         file->close( file );
      }
 }

void cat_file(char *filename){

   k_printf("Contents of %s:\n", filename);
   fs_node_t *file = fs_root->finddir(fs_root, filename);
   k_printf("Size: %d\n", file->length );

   //Allocate buffer space for the file content
   //do a sanity check to make sure the file is not
   //over 100K.
   if( file->length < 100000 && file ){
      char *buf = (char*)kmalloc( file->length, 0, 0 );
      file->read( file, 0, file->length - 1 , buf);
      k_printf(buf);  
   }

   kfree(file);
}

void sysuptime(char c){
/*
   if( c == '0' );
      system_timer.enablePrint = 0;
   else if( c == '1' )
      system_timer.enablePrint = 1;*/
}

void open_file(char *filename){

   fs_node_t *file = fs_root->finddir(fs_root, filename);

   //Allocate buffer space for the file content
   //do a sanity check to make sure the file is not
   //over 100K.
   if( file->length < 100000 ){
      char *buf = (char*)kmalloc( file->length, 0, 0 );
      file->read( file, 0, file->length, buf);

      //Write first page to screen
      int i = 0;
      int lines = 0;
      while( lines < 25 && i < file->length){
          k_printf("%c", buf[i]);
          if( buf[i] == '\n' )
              lines++;
          i++;
      }
   }
  
   kfree(file);
}

void clear_screen(){
   k_clear_screen();
   k_printf("\n\n");
}

void sysinfo(){
   k_printf("  ZZZZZZZZZZ   EEEEEE    RRRRRRRRRR      OOOOOOO      SSSSSS\n");
   k_printf("          Z    E        R         R     O       O    S      S\n");
   k_printf("         Z     E        R         R    O         O   S\n");
   k_printf("        Z      E         R        R   O           O   SS\n");
   k_printf("       Z       E           RRRRRRRR   O           O     S\n");
   k_printf("      Z        EEEEEEEE         RRR   O           O      SSSS\n");
   k_printf("     Z         E               RR R   O           O          S\n");
   k_printf("    Z          E              RR  R    O         O           S\n");
   k_printf("   Z           E             RR   R      O      O    S      S\n");
   k_printf("  ZZZZZZZZZ    EEEEEEEEEEE  RR    R       OOOOOO      SSSSSS\n");
   k_printf("                   Always First\n");
   k_printf("                   Always Best\n");
   k_printf("                   Always Powerful\n");
   k_printf("Version: V0.2\n");
   k_printf("Major features:\n");
   k_printf("Interrupts           Timer support\n");
   k_printf("Paging               Dynamic memory allocation\n");
   k_printf("VGA Modeset driver   VGA font functions\n");
   k_printf("VGA screen handling  Keyboard support w/ getline and getc\n");
   k_printf("GRUB multiboot       GRUB initrd support\n");
   k_printf("Virtual File System  Filesystem based on initrd\n");
   k_printf("Threads              Basic scheduler\n");
}

void write_file(char *filename, char *string){

   fs_node_t *file = fs_root->finddir(fs_root, filename);

   //Write the string and adjust the file length
   file->write( file, 0, file->length, string);

   kfree(file);
}  

