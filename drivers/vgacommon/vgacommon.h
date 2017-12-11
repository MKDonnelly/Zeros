#pragma once

#include <vga3h/vga3h.h>
#include <vga13h/vga13h.h>

#define VGA3H_MODE 0
#define VGA13H_MODE 1

//This is the main header for manipulating
//the screen. Based on the video mode, it 
//creates several generic functions 
//(like k_putchar and k_print) and binds
//them to several mode-specific functions
//(such as a vga 3h-mode only k_print).

struct vga_screen{

//         Cursor Manipulation

   //Return the current cursor position
   int (*cursor_position)();  

   //Move the cursor to the location (x,y)                          
   void (*move_cursor)(int x, int y);

///////////////////////////////////////////

//         Printing 
   //Put a character onto the screen
   void (*k_putchar)(char);

   //Clear the screen
   void (*k_clear_screen)();

   //Make a newline
   void (*k_newline)();

   //Print a non-formatted string
   void (*k_puts)(char*);

   //Print a formated string to the screen
   //and automatically move the cursor
   void (*k_printf)(char*, ...);

   //Print a non-formatted string 
   //at a specific location (x,y)
   void (*k_print_at)(char*,int,int);
  
////////////////////////////////////////

//         Screen capture
 
   //Capture the screen to the given
   //array. Make sure the array is
   //large enough
   void (*k_screen_cap)(int[]);

   //Restore the screenshot 
   void (*k_screen_res)(int[]);

///////////////////////////////////////

   //Scroll the screen
   void (*k_scroll)();

} kernel_vga_screen;

//These are the generic function that 
//can be called from outside of video code.
int cursor_position();
void move_cursor(int,int);
void k_putchar(char);
void k_clear_screen();
void k_newline();
void k_puts(char*);
void k_printf(char*, ...);
void k_print_at(char*,int,int);

//This does the job of binding
//the specific function to the
//generic calls.
// 0 = VGA 3h mode
// 1 = VGA 13h mode
void init_vga(uint8_t);
