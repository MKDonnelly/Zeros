#include "stdlib.h"

void evaluate(char*);

int main(){
   char msg[] = "Starting the ZerOS Shell...\n";
   syscall_write(stdout, msg, sizeof(msg));

   //Prompt user
   char prompt[] = "root@ring0-> ";
   syscall_write(stdout, prompt, sizeof(prompt));

   char linebuf[128];
   int lbidx = 0;
   char nextchar;
   while( 1 ){
      syscall_read(stdin, &nextchar, 1);

      //This skips the case where we try to do
      //a backspace when the buffer is empty
      if( ! (nextchar == '\b' && lbidx == 0) ){
         syscall_write(stdout, &nextchar, 1);
         if( nextchar == '\b' && lbidx > 0){
            linebuf[--lbidx] = 0;
         }else if( nextchar == '\r' && lbidx > 0 ){
            linebuf[lbidx] = '\0';
            evaluate(linebuf);
            mymemset(linebuf, 128, 0);
            lbidx = 0;
           
            syscall_write(stdout, prompt, sizeof(prompt));
         }else{
            linebuf[lbidx++] = nextchar;
         } 
      }
   }
   syscall_exit(0);
   return 0;
}


void eval_helper(int cur, int n, int stdinfd, char **parts){

   //Parse command into cmd, args, redirect (optional)

   char *cmd = parts[cur];
   //Move pointer forward until there are no
   //spaces
   while( cmd[0] == ' ' ) cmd++;

   char *args = cmd;
   //Move forward until we hit a space
   while( args[0] != ' ' && args[0] != 0) args++;
   args[0] = '\0';
   args++;
   //now args points to the command line arguments

   //Finally, check if there is a redirection
   char *redir_file = args;
   int redirNeeded = 0;
   while( redir_file[0] != '\0' ){
      if( redir_file[0] == '<' ){
         redirNeeded = 1;
         //Add null so that args are separated
         redir_file[0] = '\0';
         redir_file++;
         break;
      }else if( redir_file[0] == '>' ){
         redirNeeded = 2;
         redir_file[0] = '\0';
         redir_file++;
         break;
      }
      redir_file++;
   }

   //If redirection is needed, find file name
   if( redirNeeded ){
      //skip over spaces to find start of
      //filename
      while( redir_file[0] == ' ' ) redir_file++;

      //Now go to the end of the filename
      //and place a null
      char *temp = redir_file;
      while( temp[0] != ' ' && temp[0] != '\0'){
         temp++;
      }
      temp[0] = '\0';
   }
    

   //On last component, so we will have stdout
   if( cur == n ){
      char cd[] = "cd";
      if( mystrcmp(cmd, cd) == 0 ){
         //We were given an absolute path
         int ret;
         if( args[0] == '/' ){
            ret = syscall_chdir(args);
         }else{ //we were given a relative path
            char tbuf[128];
            char pwd[128];
            syscall_getpwd(pwd, 128);
            myconcat(pwd, args, tbuf);
            ret = syscall_chdir(tbuf);
         }
         if( ret == -1 ){
            char msg[] = "Error: Cannot change to directory: ";
            syscall_write(stdout, msg, mystrlen(msg));
            syscall_write(stdout, args, mystrlen(args));
            newline();
         }
      }else{

         //figure out where stdin and stdout are going
         int spawned_stdin = stdinfd;
         int spawned_stdout = stdout;
         if( redirNeeded == 1){
            //need stdin redirect

            //open file requested
            int file_input = syscall_open(redir_file);

            if( file_input == -1 ){
               char m[] = "Cannot open file: ";
               syscall_write(stdout, m, mystrlen(m));
               syscall_write(stdout, redir_file, mystrlen(redir_file));
               newline();
               return;
            }

            spawned_stdin = file_input;

         }else if( redirNeeded == 2 ){
            //need stdout redirect

            //open file requested
            int file_output = syscall_open(redir_file);

            if( file_output == -1 ){
               char m[] = "Cannot open file: ";
               syscall_write(stdout, m, mystrlen(m));
               syscall_write(stdout, redir_file, mystrlen(redir_file));
               newline();
               return;
            }

            spawned_stdout = file_output;
         }

         char temp[100];
         //Make sure to zero out temp buffer
         mymemset(temp, 100, 0); 
         char root[] = "/";
         myconcat(root, cmd, temp);

         //spawn off the binary
         void *r = (void*)syscall_spawn(temp, args, spawned_stdin, spawned_stdout);

         if( r == 0 ){
            char msg[] = "Error: Binary does not exist: ";
            syscall_write(stdout, msg, mystrlen(msg));
            syscall_write(stdout, temp, mystrlen(temp));
            newline();
         }else{
            syscall_join(r);
         }
      }
   }else{
      //Not on last component, so we need to create another pipe
      int fdin, fdout;
      syscall_makepipe(&fdin, &fdout);

      char temp[100];
      //Make sure to zero out temp buffer
      mymemset(temp, 100, 0); 
      char root[] = "/";
      myconcat(root, cmd, temp);

      void *v = (void*)syscall_spawn(temp, args, stdinfd, fdin);

      eval_helper(cur+1, n, fdout, parts);

      syscall_join(v);

      syscall_close(fdin);
      syscall_close(fdout);
   }
}

void evaluate(char *line){
   char *parts[20];
   
   int n = strsplit(parts, line, '|');

   eval_helper(0, n, stdin, parts);
}



//Create file test
/*   char root[] = "/";
   int root_fd = syscall_open(root);

   char new_file[] = "mydata";
   syscall_createf(root_fd, new_file);

   syscall_exit(0);
*/

//Write to and read back from file
/*
   char f[] = "/mydata";

   int fd = syscall_open(f);

   char data[] = "hello!";
   syscall_write(fd,  data, 7);

   //try to read back data
   char temp[10];
   syscall_read(fd, temp, 7);

   syscall_write(stdout, temp, 7);

   syscall_exit(0);
*/


//Using a pipe
/*   char msg[] = "hello";
   syscall_write(stdout, msg, sizeof(msg));

   int fdin, fdout;
   syscall_makepipe(&fdin, &fdout);

   char data[] = "hello there!";
   syscall_write(fdin, data, sizeof(data));

   char exec[] = "/echo";
   char args[] = "hello";
   syscall_spawn(exec, args, fdout, stdout);

   syscall_exit(0);
*/
