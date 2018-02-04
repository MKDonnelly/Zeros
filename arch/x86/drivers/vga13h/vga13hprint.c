#include <arch/x86/drivers/vga13h/vga13h.h>

char *VGA13H_VIDEO_MEMORY = (char*)0xA0000;

/*
//Use the drawLine function to draw a square
void drawSquare( int x, int y, int width, int height, int color){
   for(int i = 0; i < height; i++, y++){
      drawLinePoint( x, y, width, color);
   }
}*/


//Given two points (defined as structures) draw a line
//from p1 to p2 with a colored line.
void drawLine(struct point p1, struct point p2, int color){

   struct point *lowest;
   struct point *highest;

   //Vertical lines must be delt with differently
   //since the value of m is undefined.
   if( p1.x == p2.x ){
     int lowesty, highesty;
     if( p1.y < p2.y ){
       lowesty = p1.y;
       highesty = p2.y;
     }else{
       lowesty = p2.y;
       highesty = p1.y;
     }

     for(int i = lowesty; i < highesty; i++){
       int linearAddress = (int)(i * VGA13H_COLS + p1.x);
       VGA13H_VIDEO_MEMORY[ linearAddress ] = color;
     }
     return;
   }
   if( p1.x < p2.x ){
      lowest = &p1;
      highest = &p2;
   }else{
      lowest = &p2;
      highest = &p1;
   }

   //Derive the equation of the line
   //using y = mx + b where m =
   //delta y / delta x
   double deltay = highest->y - lowest->y;
   double deltax = highest->x - lowest->x;
   double m = deltay / deltax;

   //Determine b, which equals y - mx
   double b = highest->y - m * highest->x;

   //Increment x by .1 so that large changes in y values
   //does not break the line
   for(double i = (double)lowest->x; i < highest->x; i += .1){
      //i is acting as x
      int ycoord = (int)(m * i + b);
      //int linearAddress = i * VGA13H_COLS + ycoord;
      int linearAddress = (int)(ycoord * VGA13H_COLS + i);
      VGA13H_VIDEO_MEMORY[ linearAddress ] = color;
   }
}

//Wrapper around drawLine function to allow inputting points
//instead of point structs.
void drawLinePoint(int p1x, int p1y, int p2x, int p2y, int color){
   struct point p1 = { .x = p1x, .y = p1y};
   struct point p2 = { .x = p2x, .y = p2y};
   drawLine(p1, p2, color);
}
