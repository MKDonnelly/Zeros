#include <lib/print.h>

#include <arch/current_arch>
#include <lib/types.h>
#include <lib/string.h>

//TODO Fails when we print two chars in a row
//     i.e. k_printf("%c %c", 'Z', 'Z');
//     Only the first char is printed
void k_printf(char *str, ...){

   va_arg args = va_start( str );

   int i = 0;
   while( str[i] != 0 ){
      //Used to store result
      char buf[20];

      if( str[i] == '%' ){

         if( str[i+1] == 'd' ){
            int arg = va_get( args, int );
            itoa( arg, buf );
         }else if( str[i+1] == 'x' ){
            int arg = va_get( args, int );
            itoh( arg, buf );
         }else if( str[i+1] == 's' ){
            char *s = va_get( args, char* );
            strncpy( buf, s, 20 ); 
         }else if( str[i+1] == 'c' ){
            //NOTE: Even though a char is 1 byte,
            //pushing it onto the stack makes it 4 bytes
            //due to stack alignment. va_get must then
            //advance 4 bytes to get the next char.
            char c = va_get(args, uint32_t);
            buf[0] = c;
            buf[1] = 0;
         } 

        //Print string representation
        k_puts(buf);
        i += 2;
      }else{
         buf[0] = str[i];
         buf[1] = 0;
         k_puts(buf);
         i++;
      }
   }
}
