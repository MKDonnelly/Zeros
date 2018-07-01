#include <arch/x86/drivers/vga3h/vga3h.h>

#include <lib/string.h>

#define VGA3H_NEWLINE 10
#define VGA3H_ROWS 25
#define VGA3H_COLS 80

//Each block seen on the screen has two cells
//associated with it, the character and the color
#define VGA3H_COLCELLS (80 * 2)
#define VGA3H_ROWCELLS (25 * 2)

#define SCREEN_CTRL_P 0x3D4
#define SCREEN_DATA_P 0x3D5

//Cursor ports
#define VGA3H_CURSOR_HIGH_C 14
#define VGA3H_CURSOR_LOW_C  15

vmode_t vmode_vga3h = {
   .vmode_putstr = vga3h_putstr,
   .vmode_putstr_at = vga3h_putstr_at,
   .vmode_init      = vga3h_init,
};

static char *vidmem = (char*)0xB8000;

static int screen_offset = 0;
static int cursor_offset = 0;
static int fg_color = COLOR_WHITE;
static int bg_color = COLOR_BLACK;

void vga3h_init(vmode_t *mode){
   vidmem += mode->kernel_vbase;
}

static inline int vga3h_xy_to_linear(int x, int y){
   return y * VGA3H_COLCELLS + x;
}

static void vga3h_move_cursor_linear(int linear_offset){
   portb_write(SCREEN_CTRL_P, VGA3H_CURSOR_HIGH_C);
   portb_write(SCREEN_DATA_P, linear_offset >> 8); //send upper bits
   portb_write(SCREEN_CTRL_P, VGA3H_CURSOR_LOW_C);
   portb_write(SCREEN_DATA_P, linear_offset);      //send lower bits
}

static void vga3h_move_cursor( int x, int y ){
   vga3h_move_cursor_linear( vga3h_xy_to_linear( x, y ) );
}


static inline char vga3h_make_color(char fg, char bg){
   return fg | bg << 4;
}


static void vga3h_scroll(){
    int max = VGA3H_ROWS * VGA3H_COLS * 2;

   //Since each character is 80 spaces (160 slots)
   //away from the space directly above, we can copy 
   //the memory at slot 160 to 0, 161 to 1 ... up to the end.
   int to, from;
   for(to = 0, from = (VGA3H_COLS*2); to <= max; to++, from++){
      vidmem[to] = vidmem[from];
   }

   screen_offset -= VGA3H_COLS * 2;
   cursor_offset -= VGA3H_COLS;
   vga3h_move_cursor_linear( cursor_offset );
}

static void vga3h_newline(){
   //Divide the current screen offset by the size of a row
   int current_line = screen_offset / VGA3H_COLCELLS;

   //Add 1 to go to the next line
   screen_offset = (current_line + 1) * VGA3H_COLCELLS;

   cursor_offset = (current_line + 1) * VGA3H_COLS;
   vga3h_move_cursor_linear( cursor_offset );
}

static void vga3h_clear_screen(){
   char color = vga3h_make_color( COLOR_WHITE, COLOR_BLACK );
   for(int start = 0; start < VGA3H_ROWS * VGA3H_COLS * 2; start += 2){
      vidmem[start] = ' ';
      vidmem[start+1] = color;
   }
   screen_offset = 0;
   vga3h_move_cursor_linear(screen_offset);
}

//TODO get strstr working in string.c/h
int next_char_of(char c, char *str){
   int i = 0;
   while(str[i] != c)
      i++;
   return i;
}


void vga3h_putstr(char *str){

   int i = 0;
   while( str[i] != 0 ){

      //Handle text rolling off the end of the screen
      if( screen_offset >= VGA3H_ROWS * VGA3H_COLS * 2 ){
         vga3h_scroll();
      }

      switch( str[i] ){

         case '\n':
         case '\r':
            vga3h_newline();
            i++;
            break;

         case '\b':
            //Move back one cell and blank out the character
            screen_offset -= 2;
            cursor_offset--;
            vidmem[screen_offset] = ' ';
            vga3h_move_cursor_linear(cursor_offset);
            i++;
            break;

         //Special escape sequence to clear screen
         case '\\':
            i++;
            if( str[i] == 'c' ){
               vga3h_clear_screen();
               i++;
            }else if( str[i] == 'm' && str[i+1] == '{' ){
               i += 2;
               int xval = atoi( &str[i] );
               i += next_char_of( ',', &str[i] ) + 1;
               int yval = atoi( &str[i] );
               i += next_char_of( '}', &str[i] ) + 1;
               cursor_offset = vga3h_xy_to_linear( xval, yval );
               vga3h_move_cursor( xval, yval );
               screen_offset = (xval * VGA3H_COLS + yval) * 2;
            }else if( str[i] == 's' ){
               vga3h_scroll();
               i++;
            }else if( str[i] == 'g' && str[i+1] == '{' ){
               i += 2;
               int color = atoi( &str[i] );
               if( color >= 0 && color <= 15 )
                  bg_color = color;
               i += next_char_of( '}', &str[i] ) + 1;
            }else if( str[i] == 'f' && str[i+1] == '{' ){
               i += 2;
               int color = atoi( &str[i] );
               if( color >= 0 && color <= 15 )
                  fg_color = color;
               i += next_char_of( '}', &str[i] ) + 1;
            }
            break;
   
         //Print a regular character
         default:
            vidmem[screen_offset] = str[i];
            vidmem[screen_offset+1] = vga3h_make_color(fg_color, bg_color);
            screen_offset += 2;
            cursor_offset += 1;
            vga3h_move_cursor_linear(cursor_offset);
            i++;
            break;
      }
   }
}

void vga3h_putstr_at(char *str, int xval, int yval){

   //Multiply by 2 since there are 2 bytes per ever cell seen
   int start_at = (yval * VGA3H_COLS + xval) * 2;

   int i = 0;
   while( str[i] != 0 ){

      int line_number;
      switch( str[i] ){

         case '\n':
         case '\r':
            line_number = start_at / (VGA3H_COLS * 2);
            start_at = ( line_number + 1) * VGA3H_COLS * 2;
            i++;
            break;

         case '\b':
            //Move back one cell and blank out the character
            start_at -= 2;
            vidmem[start_at] = ' ';
            i++;
            break;

         //Special escape sequence to clear screen
         case '\\':
            i++;
            if( str[i] == 'c' ){
               vga3h_clear_screen();
               i++;
            }else if( str[i] == 'm' && str[i+1] == '{' ){
               i += 2;
               int xval = atoi( &str[i] );
               i += next_char_of( ',', &str[i] ) + 1;
               int yval = atoi( &str[i] );
               i += next_char_of( '}', &str[i] ) + 1;
               start_at = (yval * VGA3H_COLS + xval) * 2;
            }else if( str[i] == 's' ){
               //vga3h_scroll();
               i++;
            }else if( str[i] == 'g' && str[i+1] == '{' ){
               i += 2;
               int color = atoi( &str[i] );
               if( color >= 0 && color <= 15 )
                  bg_color = color;
               i += next_char_of( '}', &str[i] ) + 1;
            }else if( str[i] == 'f' && str[i+1] == '{' ){
               i += 2;
               int color = atoi( &str[i] );
               if( color >= 0 && color <= 15 )
                  fg_color = color;
               i += next_char_of( '}', &str[i] ) + 1;
            }
            break;
   
         //Print a regular character
         default:
            vidmem[start_at] = str[i];
            vidmem[start_at+1] = vga3h_make_color(fg_color, bg_color);
            start_at += 2;
            i++;
            break;
      }
   }
}
