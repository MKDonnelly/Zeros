#include <arch/x86/drivers/vgacommon/vgacommon.h>

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

void k_putchar_at(char character, int x, int y){
   kernel_vga_screen.k_putchar_at( character, x, y );
}

void k_clear_screen(){
   kernel_vga_screen.k_clear_screen();
}

void k_newline(){
   kernel_vga_screen.k_newline();
}

//TODO Fails when we print two chars in a row
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
          k_putchar_at( buf[j++], x++, y ); 
        i += 2;
      }else{
         k_putchar_at( str[i], x++, y );
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

void k_scroll_screen(){
   kernel_vga_screen.k_scroll();
}

void k_set_bg_color(int back){
   kernel_vga_screen.k_set_bg_color(
         kernel_vga_screen.vga_colors[back] );
}

void k_set_fg_color(int fore){
   kernel_vga_screen.k_set_fg_color( 
         kernel_vga_screen.vga_colors[fore] );
}

int k_get_bg_color(){
   return kernel_vga_screen.k_get_bg_color();
}

int k_get_fg_color(){
   return kernel_vga_screen.k_get_fg_color();
}

void init_vga(uint8_t mode){

   if( mode == 0 ){

      kernel_vga_screen.cursor_position = vga3h_get_cursor;
      kernel_vga_screen.move_cursor = vga3h_move_cursor;

      kernel_vga_screen.k_putchar = vga3h_putchar;
      kernel_vga_screen.k_putchar_at = vga3h_putchar_at;
      kernel_vga_screen.k_clear_screen = vga3h_fast_cls;
      //kernel_vga_screen.k_clear_screen = vga3h_clear_screen;
      kernel_vga_screen.k_newline = vga3h_newline;

      kernel_vga_screen.k_screen_cap = vga3h_screen_cap;
      kernel_vga_screen.k_screen_res = vga3h_screen_res;

      kernel_vga_screen.k_scroll = vga3h_scroll;

      //Map the colors
      kernel_vga_screen.vga_colors = vga3h_color_array;

      kernel_vga_screen.k_set_bg_color = vga3h_set_bg_color;
      kernel_vga_screen.k_set_fg_color = vga3h_set_fg_color;
      kernel_vga_screen.k_get_bg_color = vga3h_get_bg_color;
      kernel_vga_screen.k_get_fg_color = vga3h_get_fg_color;

   }else if( mode == 1){
      //TODO VGA 13h mode
   }
}
