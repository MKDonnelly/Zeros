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

