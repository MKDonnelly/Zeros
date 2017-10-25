#include "vgatext.h"

//Can set both the forground and background color of
//text
unsigned char make_color(enum vga_color fg, enum vga_color bg){
    return fg | bg << 4;
}

//Smashes the color and the character of
//a printable character together
unsigned short int make_vgaentry(char c, unsigned char color){
    unsigned short int c16 = c;
    unsigned short int color16 = color;
    return (c16 | color16 << 8);
};

