
#include <vgacommon.h>

int cursor_position(){
   return kernel_vga_screen.cursor_position();
}

void move_cursor(int x, int y){
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

void k_printf(char *string){
   kernel_vga_screen.k_printf( string );
}

void k_print_at(char *string, int x, int y){
   kernel_vga_screen.k_print_at( string, x, y );
}

void init_vga(uint8_t mode){
   if( mode == 0 ){
      kernel_vga_screen.cursor_position = vga3h_get_cursor;
      kernel_vga_screen.move_cursor = vga3h_move_cursor;

      kernel_vga_screen.k_putchar = vga3h_putchar;
      kernel_vga_screen.k_clear_screen = vga3h_clear_screen;
      kernel_vga_screen.k_newline = vga3h_newline;
      kernel_vga_screen.k_printf = vga3h_print;
      kernel_vga_screen.k_print_at = vga3h_print_at;

      //TODO
      //kernel_vga_screen.k_screen_cap = vga3h_screen_cap;
      //kernel_vga_screen.k_screen_res = vga3h_screen_res;

      kernel_vga_screen.k_scroll = vga3h_scroll;
   }else if( mode == 1){

   }
}
