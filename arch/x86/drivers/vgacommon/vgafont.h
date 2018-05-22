#pragma once 

#include <lib/types.h>
//Example: write_font( g_8x16_font, 16 );
unsigned char g_8x16_font[4096];
void write_font(unsigned char *font_buf, unsigned font_height);
