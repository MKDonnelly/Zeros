/*//TODO Fails when we print two chars in a row
//     i.e. k_printf("%c %c", 'Z', 'Z');
//     Only the first char is printed
void k_printf(char *str, ...){

   va_arg args = va_start( str );

   int i = 0;
   while( str[i] != 0 ){

      if( str[i] == '%' ){
         //Used to store result
         char buf[20];

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
            char c = va_get(args, char);
            buf[0] = c;
            buf[1] = 0;
         } 

        //Print string representation
        int j = 0;
        while( buf[j] != 0 )
          k_putchar( buf[j++] ); 
        i += 2;
      }else{
         k_putchar( str[i] );
         i++;
      }
   }
}*/
