#pragma once

#include <portio.h>

#define VGA3H_NEWLINE 10
#define VGA3H_ROWS 25
#define VGA3H_COLS 80

#define SCREEN_CTRL_PORT 0x3D4
#define SCREEN_DATA_PORT 0x3D5


//Initilized in print.c
extern char *VGA3H_VIDEO_MEMORY;
extern int VGA3H_CUR_SCREEN_OFFSET;

//cursor.c functions
int vga3h_get_cursor();
void vga3h_move_cursor(int,int);
void vga3h_move_cursorl(int);
int k_xy_to_linear(int,int);


//print.c functions
void vga3h_putchar(char);
void vga3h_newline();
void vga3h_print(char*);
void vga3h_print_at(char*,int,int);
void vga3h_clear_screen();

//screencap.c functions
void vga3h_screen_cap(int captureArray[][160]);
void vga3h_screen_res(int restoreArray[][160]);

//scroll.c functions
void vga3h_scroll();
void vga3h_scroll_by(int);


//vgaentry.c functions
//The colors capable of being printed
enum vga3h_color{
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15,
};

unsigned char vga3h_make_color(enum vga3h_color , enum vga3h_color );
unsigned short int vga3h_make_vgaentry(char , unsigned char );
