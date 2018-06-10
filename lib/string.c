#include <lib/string.h>

//Basic function to copy and set memory
void memcpy( void *dest, void *src, size_t amount){
   for(int i = 0; i < amount; i++)
      ((char*)dest)[i] = ((char*)src)[i];
}

//Overwrite *dest with amount bytes of value val
void memset( void *dest, size_t amount, int val){
   for(int i = 0; i < amount; i++)
      ((char*)dest)[i] = val;
}


//Find length of string
size_t strlen(char inputarray[]){
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
void strncpy( char *str1, char *str2, size_t len){
   size_t i = 0;
   while( str2[i] != 0 && i < len){
      str1[i] = str2[i];
      i++;
   }
   str1[i] = 0;
}

int strcmp(char *str1, char *str2){
   size_t i = 0;
   while( ! (str1[i] - str2[i]) && str1[i] != 0 && str2[i] != 0)
      i++;
   return (str1[i] - str2[i]);
}

int strncmp(char *str1, char *str2, size_t len){
   for(size_t i = 0; i < len; i++){
      if( str1[i] != str2[i] )
         return (str1[i] - str2[i]);
   }
   return 0;
}


char *strchr(char *str, char c){
   size_t i = 0;
   while( str[i] != 0 ){
      if( str[i] == c )
         return &str[i];
   }
   return 0;
}

//We will assume that str1 has enough space
//to place str2 at the end.
void strcat(char *str1, char *str2){
   size_t s1len = strlen(str1);
   size_t s2len = strlen(str2);
   for(int j = 0; j < s2len; j++){
      str1[s1len+j] = str2[j];
   }
}


char *strstr(char *source, char *substr){
   size_t source_len = strlen(source);
   for(int i = 0; i < source_len; i++){
      if( strcmp( &source[i], substr ) == 0 )
         return &source[i];
   }
   return 0;
}

//Reverse the given string
void reverse(char s[]){
   int i, j;
   char c;

   for(i = 0, j = strlen(s)-1; i < j ; i++, j--){
      c = s[i];
      s[i] = s[j];
      s[j] = c;
   }
}

int is_in(char c, char *str){
   int present = 0;
   for(int i = 0; str[i] != 0; i++){
      if( c == str[i] )
         present = 1;
   }
   return present;
}

//Converts a string into an integer
int atoi(char *str){
   int result = 0;
   size_t i = 0;
   while( is_in( str[i], "0123456789" ) ){
      result *= 10;
      result += (str[i] - '0');
      i++;
   }
   if( str[0] == '-' )
      result = -result;
   return result;
}


//Convert the integer n into 
//an ascii string.
void itoa(int number, char *buf){
   int i, sign;

   if( (sign = number) < 0)
      number = -number;
   i = 0;
   do{
      buf[i++] = number % 10 + '0';
   }while( (number /= 10) > 0);
   if(sign < 0)
      buf[i++] = '-';
   buf[i] = '\0';
   reverse(buf);
}

void itoh(int number, char *buf){

   char hex_digits[] = "0123456789ABCDEF";

   for(size_t i = 0; i < 8; i++){
      int val = number & 0xF;
      buf[i] = hex_digits[val];
      number = number >> 4;
   }
   //Add the null bit
   buf[8] = 0;
   reverse(buf);
}
