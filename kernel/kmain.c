asm("jmp kmain"); //The bootsector immedietelly jumps to the
                  //start of this file and begins executing. This is
		  //undesirable if there is a function above kmain.
		  //add this so that kmain is always called.

#include "../drivers/portio.h"
#include "../drivers/timing.h"
#include "../drivers/cpu.h"
#include "../lib/string.h"
#include "../drivers/console.h"

#define LINES 200
#define COLS 320
#define LIGHT_GREEN 10


char *VIDEO = (char*)0xA0000;

void drawLine( int x, int y, int len, int color ){
   int startPosition = y * COLS + x;
   for(int i = startPosition; i < startPosition + len; i++)
      VIDEO[i] = color;
}

void drawSquare( int x, int y, int width, int height, int color){
   for(int i = 0; i < height; i++, y++){
      drawLine( x, y, width, color);

   }
}

struct point{
   int x, y;
};

//TODO needs work
void drawLine2( struct point p1, struct point p2, int color){

   //Find the smallest point. The smallest point is
   //the point with the smallest x-coordinate, or if
   //the xes are equal, the smallest y-coordinate.
   struct point smallest, largest;
   if( p1.x < p2.x ){
      smallest = p1;
      largest = p2;
   }else if( p1.x > p2.x ){
      smallest = p2;
      largest = p1;
   }else{
      if( p1.y < p2.y ){
        smallest = p1;
	largest = p2;
      }else{
        smallest = p2;
	largest = p1;
      }
   }

   //Handle vertical and horizontal lines
   if( largest.x == smallest.x ){
      for(int i = smallest.y; i <= largest.y; i++){
         VIDEO[ i * COLS + p1.x ] = color;
      }
      return;
   }else if( largest.y == smallest.y ){
      for(int i = smallest.x; i <= largest.x; i++){
         VIDEO[ p1.y * COLS + i ] = color;
      }
      return;
   }


   //Solve y = mx + b to find the equation of the line
   //If either the x coordinates are the same, or the
   //y coordinates are the same, a horizontal or vertical
   //line is drawn, respectively. The case of a vertical
   //line must be caught as the slope is undefined.
   double deltax = (largest.x + smallest.x) / 2.;
   double deltay = (largest.y + smallest.y) / 2.;
   double m = deltay / deltax;

   //find b
   double b = largest.y - m * smallest.y;

   for(int i = smallest.x; i <= largest.x; i++){
      //i is acting as the x coordinate
      //put it into our y = mx + b formula
      //to find the y coordinate
      int ycoord = (int)(m * i + b);
      int linearAddress = ycoord * COLS + i;
      VIDEO[ linearAddress ] = color;  
   }
}

void drawLine3(struct point p1, struct point p2, int color){
   
   //Make sure drawing starts from the smallest point.
   struct point largest, smallest;
   if( p1.x < p2.x ){
      smallest = p1;
      largest = p2;
   }else if( p1.x > p2.x){
      smallest = p2;
      largest = p1;
   }else{
      if( p1.y > p2.y ){
         smallest = p2;
	 largest = p1;
      }else{
         smallest = p1;
	 largest = p2;
      }
   }

   //A vertical line must be handled first, as the slope
   //is undefined.
   if( p1.x == p2.x ){
      for( int i = smallest.y; i <= largest.y; i++){
         VIDEO[ i * COLS + p1.x ] = color;
      }
   //Non-vertical line
   }else{

      //Solve y = mx + b to find the equation of the line
      //If either the x coordinates are the same, or the
      //y coordinates are the same, a horizontal or vertical
      //line is drawn, respectively. The case of a vertical
      //line must be caught as the slope is undefined.
      double deltax = (largest.x + smallest.x) / 2.;
      double deltay = (largest.y + smallest.y) / 2.;
      double m = deltay / deltax;

      //find b
      double b = largest.y - m * smallest.y;

      for(int i = smallest.x; i <= largest.x; i++){
         //i is acting as the x coordinate
         //put it into our y = mx + b formula
         //to find the y coordinate
         int ycoord = (int)(m * i + b);
         int linearAddress = i * COLS + ycoord;
         VIDEO[ linearAddress ] = color;  
      }
   }
}


void kmain(){

  struct vga_text_console sys_console;

  initConsole( &sys_console );
  sys_console.k_print("Testing...");

  stop_cpu();

}

