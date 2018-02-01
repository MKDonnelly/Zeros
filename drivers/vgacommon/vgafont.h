#pragma once 

#include <portio.h>
#include <modeset.h>
#include <memory.h>
#include <types.h>
/*
   Example:
      write_font( g_8x16_font, 16 );
*/

unsigned char g_8x16_font[4096];
void write_font(unsigned char *buf, unsigned font_height);
