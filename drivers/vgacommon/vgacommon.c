
#include <vgacommon.h>

//These are meant to be called by external code
int k_cursor_position(){
   return kernel_vga_screen.cursor_position();
}

void k_move_cursor(int x, int y){
   kernel_vga_screen.move_cursor(x, y);
}

void k_putchar(char character){
   kernel_vga_screen.k_putchar( character );
}

void k_clear_screen(){
   kernel_vga_screen.k_clear_screen();
}

void k_newline(){
   kernel_vga_screen.k_newline();
}

void k_puts(char *string){
   int i = 0;
   while( string[i++] != 0 )
      kernel_vga_screen.k_putchar( string[i] );
}

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
}


void k_printf_at(char *str, int x, int y, ...){

   va_arg args = va_start( y );

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
         }

        //Print string representation
        int j = 0;
        while( buf[j] != 0 )
          kernel_vga_screen.k_putchar_at( buf[j++], x++, y ); 
        i += 2;
      }else{
         kernel_vga_screen.k_putchar_at( str[i], x++, y );
         i++;
      }
   }
}


void k_vga_capture(char *captureArray){
   kernel_vga_screen.k_screen_cap( captureArray );
}

void k_vga_restore(char *restoreArray){
   kernel_vga_screen.k_screen_res( restoreArray );
}


void init_vga(uint8_t mode){

   if( mode == 0 ){
      kernel_vga_screen.cursor_position = vga3h_get_cursor;
      kernel_vga_screen.move_cursor = vga3h_move_cursor;

      kernel_vga_screen.k_putchar = vga3h_putchar;
      kernel_vga_screen.k_putchar_at = vga3h_putchar_at;
      kernel_vga_screen.k_clear_screen = vga3h_clear_screen;
      kernel_vga_screen.k_newline = vga3h_newline;

      kernel_vga_screen.k_screen_cap = vga3h_screen_cap;
      kernel_vga_screen.k_screen_res = vga3h_screen_res;

      kernel_vga_screen.k_scroll = vga3h_scroll;
   }else if( mode == 1){
      //TODO VGA 13h mode
   }
}


