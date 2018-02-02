#include <lib/string.h>

//Find length of string
int strlen(char inputarray[]){
   int i = 0;
   while( inputarray[i] != '\0')
      i++;
   return i;
}

//Copy str2 (a valid c-string) into 
//str1.
void strcpy( char *str1, char *str2 ){
   int i = 0;
   while( str2[i] != 0 ){
      str1[i] = str2[i];
      i++;
   }
   //Make sure to set the null byte
   str1[i] = 0;
}

//Copies str2 into str1, copying no
//more than num bytes
void strncpy( char *str1, char *str2, int num){
   int i = 0;
   while( str2[i] != 0 && i < num){
      str1[i] = str2[i];
      i++;
   }
   str1[i] = 0;
}

int strcmp(char *str1, char *str2){
   int i = 0;
   while( ! (str1[i] - str2[i]) && str1[i] != 0 && str2[i] != 0)
      i++;
   return (str1[i] - str2[i]);
}

//We will assume that str1 has enough space
//to place str2 at the end.
void strcat(char *str1, char *str2){
   int s1len = strlen(str1);
   int s2len = strlen(str2);
   for(int j = 0; j < s2len; j++){
      str1[s1len+j] = str2[j];
   }
}

/*
char *strstr(char *source, char *substr){
   int source_len = strlen(source);
   int substr_len = strlen(substr);
   char *ret = NULL;

   int i = 0;
   while( ! ret && i < source_len && i < substr_len ){
      int j = 0;
      while( source[i] == substr[j] ) j++;
      if( j == substr_len )
         ret = source[i];
   }
}*/

//Reverse the given string
void reverse(char s[]){
   int i, j;
   char c;

   for(i = 0, j = strlen(s)-1; i< j ; i++, j--){
      c = s[i];
      s[i] = s[j];
      s[j] = c;
   }
}

//Convert the integer n into 
//an ascii string.
void itoa(int n, char s[]){
   int i, sign;

   if( (sign = n) < 0)
      n = -n;
   i = 0;
   do{
      s[i++] = n % 10 + '0';
   }while( (n /= 10) > 0);
   if(sign < 0)
      s[i++] = '-';
   s[i] = '\0';
   reverse(s);
}

void itoh(int num, char s[]){

   char hex_digits[] = "0123456789ABCDEF";

   for(int i = 0; i < 8; i++){
      int val = num & 0xF;
      s[i] = hex_digits[val];
      num = num >> 4;
   }
   //Add the null bit
   s[8] = 0;
   reverse(s);
}
