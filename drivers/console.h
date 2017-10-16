
#pragma once

#include "vgaentry.h"

//A console device to be used by the system.
//this aggregates most functions that alter
//the screen.
struct vga_text_console{

   void (*k_putchar)(char);
   void (*k_clear_screen)(void);
   void (*k_newline)(void);
   
   void (*k_printfull)(char*,enum vga_color);
   void (*k_print)(char*);

   void (*k_screen_cap)(int[][160]);
   void (*k_screen_res)(int[][160]);

   int (*get_cursor)(void);
   void (*move_cursor)(int, int);
   void (*move_cursorl)(int);
   int (*k_xy_to_linear)(int,int);

   void (*scroll)(void);
   void (*scroll_by)(int);
};

void initConsole( struct vga_text_console *);

