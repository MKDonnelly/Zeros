#pragma once

#include "vgaentry.h"

void k_putchar(char);
void k_newline();
void k_printfull(char *msg);//, enum vga_color color);
void k_print(char *msg);
void k_clear_screen();

int get_cursor();
void move_cursor(int, int);
