
#include "print.h"
#include "vgaentry.h"
#include "screencap.h"
#include "cursor.h"
#include "scroll.h"
#include "console.h"

void initConsole(struct vga_text_console *dev){
   
   dev->k_putchar      = &k_putchar;         //Defined in libprint
   dev->k_clear_screen = &k_clear_screen;    //Defined in libprint
   dev->k_newline      = &k_newline;         //Defined in libprint

   dev->k_printfull    = &k_printfull;       //Defined in libprint
   dev->k_print        = &k_print;           //Defined in libprint

   dev->k_screen_cap   = &k_screen_cap;      //Defined in libscreencap
   dev->k_screen_res   = &k_screen_res;      //Defined in libscreencap

   dev->get_cursor     = &get_cursor;        //Defined in libcursor
   dev->move_cursor    = &move_cursor;       //Defined in libcursor
   dev->move_cursorl   = &move_cursorl;      //Defined in libcursor
   dev->k_xy_to_linear = &k_xy_to_linear;    //Defined in libcursor

   dev->scroll         = &scroll;            //Defined in libscroll
   dev->scroll_by      = &scroll_by;         //Defined in libscroll
}


