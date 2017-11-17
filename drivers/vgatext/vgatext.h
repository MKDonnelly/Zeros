#pragma once

#include "../../drivers/portio.h"

#define NEWLINE 10
#define ROWS 25
#define COLS 80

#define SCREEN_CTRL_PORT 0x3D4
#define SCREEN_DATA_PORT 0x3D5


//Initilized in print.c
extern char *VIDEO_MEMORY;
extern int CUR_SCREEN_OFFSET;

//cursor.c functions
int get_cursor();
void move_cursor(int,int);
void move_cursorl(int);
int k_xy_to_linear(int,int);


//print.c functions
void k_putchar(char);
void k_newline();
void k_print(char*);
void k_print_at(char*,int,int);
void k_clear_screen();

//screencap.c functions
void k_screen_cap(int captureArray[][160]);
void k_screen_res(int restoreArray[][160]);

//scroll.c functions
void scroll();
void scroll_by(int);


//vgaentry.c functions
//The colors capable of being printed
enum vga_color{
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

unsigned char make_color(enum vga_color , enum vga_color );
unsigned short int make_vgaentry(char , unsigned char );
