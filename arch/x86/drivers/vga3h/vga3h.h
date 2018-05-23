#pragma once

#include <arch/x86/portio.h>
#include <arch/x86/drivers/vgacommon/vgacommon.h>

extern vmode_t vmode_vga3h;

//The color codes for vga3h mode. 
//Use with vga3h_make_color 
enum VGA3H_COLORS {
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

void vga3h_putstr(char *str);
void vga3h_putstr_at(char *str, int x, int y);
