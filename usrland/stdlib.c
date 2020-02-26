#include "stdlib.h"

int strsplit(char **parts, char *line, char spliton){
   int partnum = 0;
   char *start = line;
   int slen = mystrlen(line);

   //Initialize first part as the start of the string
   parts[partnum] = start;

   int i = 0;
   while( i < slen ){
      if( line[i] == spliton ){
         parts[partnum++] = start;
         line[i] = '\0';
         start = &line[i+1];
      }
      i++;
   }
   parts[partnum] = start;
   return partnum;
}

int mystrlen(char inputarray[]){
   int i = 0;
   while( inputarray[i] != '\0')
      i++;
   return i;
}

//Overwrite *dest with amount bytes of value val
void mymemset( void *dest, int amount, int val){
   for(int i = 0; i < amount; i++)
      ((char*)dest)[i] = val;
}

void newline(){
   char nl = '\n';
   syscall_write(stdout, &nl, 1);
}

void mystrcpy(char *dest, char *src){
   int i = 0;
   while( src[i] != 0 ){
      dest[i] = src[i];
      i++;
   }
   dest[i] = 0;
}

int mystrcmp(char *str1, char *str2){
   int i = 0;
   while( ! (str1[i] - str2[i]) && str1[i] != 0 && str2[i] != 0)
      i++;
   return (str1[i] - str2[i]);
}

void myconcat(char *pre, char *post, char *buf){
   int pre_len = mystrlen(pre);
   int post_len = mystrlen(post);
   int buf_i = 0;
   for(int i = 0; i < pre_len; i++){
      buf[buf_i++] = pre[i];
   }
   for(int i = 0; i < post_len; i++){
      buf[buf_i++] = post[i];
   }
}

//Makes absolute path from relative path
void makeapath(char *relpath, char *buf){
   //Catch the case where relpath is actuall
   //an absolute path
   if( relpath[0] == '/' ){
      mystrcpy(buf, relpath);
      return;
   }

   //First, get pwd
   char pwd[128];
   syscall_getpwd(pwd, 128);

   //Make sure there is a '/' at the end
   int pwd_len = mystrlen(pwd);
   if( pwd[pwd_len-1] != '/' ){
      pwd[pwd_len] = '/';
      pwd[pwd_len+1] = '\0';
   }

   //Now concatenate pwd and relative path
   myconcat(pwd, relpath, buf);
}
