#pragma once

#include <arch/x86/portio.h>

#define VGA3H_NEWLINE 10
#define VGA3H_ROWS 25
#define VGA3H_COLS 80

#define SCREEN_CTRL_P 0x3D4
#define SCREEN_DATA_P 0x3D5

//Cursor ports
#define VGA3H_CURSOR_HIGH_P 14
#define VGA3H_CURSOR_LOW_P  15

//Initilized in print.c
extern char *VGA3H_VIDEO_MEMORY;
extern int VGA3H_CUR_SCREEN_OFFSET;
extern int VGA3H_CUR_CURSOR_OFFSET;
extern int VGA3H_CUR_FG_COLOR;
extern int VGA3H_CUR_BG_COLOR;

//cursor.c functions
int vga3h_get_cursor();
void vga3h_move_cursor(int,int);
void vga3h_move_cursorl(int);
int vga3h_xy_to_linear(int,int);

//print.c functions
void vga3h_putchar(char);
void vga3h_putchar_at(char,int,int);

void vga3h_newline();
void vga3h_clear_screen();
void vga3h_fast_cls();

//color handling
void vga3h_set_fg_color(int);
void vga3h_set_bg_color(int);
int vga3h_get_fg_color();
int vga3h_get_bg_color();

//screencap.c functions
void vga3h_screen_cap(char *captureArray);
void vga3h_screen_res(char *restoreArray);

//scroll.c function
void vga3h_scroll();

//The color codes for vga3h mode. 
//Use with vga3h_make_color 
enum vga3h_color{
    VGA3H_BLACK = 0,
    VGA3H_BLUE = 1,
    VGA3H_GREEN = 2,
    VGA3H_CYAN = 3,
    VGA3H_RED = 4,
    VGA3H_MAGENTA = 5,
    VGA3H_BROWN = 6,
    VGA3H_LIGHT_GREY = 7,
    VGA3H_DARK_GREY = 8,
    VGA3H_LIGHT_BLUE = 9,
    VGA3H_LIGHT_GREEN = 10,
    VGA3H_LIGHT_CYAN = 11,
    VGA3H_LIGHT_RED = 12,
    VGA3H_LIGHT_MAGENTA = 13,
    VGA3H_LIGHT_BROWN = 14,
    VGA3H_WHITE = 15,
};

//This is the color interface for the abstracted
//vgacommon code. In vgacommon, we give common colors
//values (i.e. black is 0). Here, we could place that
//value in and get the mode-specific code for that color.
extern char vga3h_color_array[];

static inline char vga3h_make_color(char fg, char bg){
   return fg | bg << 4;
}

