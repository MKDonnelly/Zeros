#pragma once 

#include <arch/x86/portio.h>
#include <arch/x86/drivers/vgacommon/modeset.h>
#include <lib/memory.h>
#include <lib/types.h>
/*
   Example:
      write_font( g_8x16_font, 16 );
*/

unsigned char g_8x16_font[4096];
void write_font(unsigned char *buf, unsigned font_height);
