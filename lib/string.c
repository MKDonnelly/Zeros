#include "string.h"

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
   str1[i] = '\0';
}

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

