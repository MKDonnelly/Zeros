#pragma once

#include <lib/string.h>

#define VGA13H_LINES 200
#define VGA13H_COLS 320
#define LIGHT_GREEN 10

struct point{
   int x;
   int y;
};

//Initilized in vga13hprint.c
extern char *VGA13H_VIDEO_MEMORY;

void drawLine( struct point, struct point, int color);
void drawLinePoint(int x1,int y1,int x2,int y2,int color);
void drawSquare( int x, int y, int width, int height, int color);

