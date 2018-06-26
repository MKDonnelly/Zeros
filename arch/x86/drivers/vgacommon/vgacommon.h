#pragma once

/*      Set of escape sequences that each mode must understand
    \c = Clear screen
    \m{<x>,<y>} move cursor to (<x>, <y>)
    \s = Scroll down one line
    \g{<color>} = Set background to <color> (see defined constants)
    \f{<color>} = Set foreground to <color> (see defined constants)
*/

//Structure describing a specific video mode
typedef struct vmode{
   //Prints a string to the screen
   void (*vmode_putstr)(char*);

   //Prints a string to the screen at a certain (x, y)
   void (*vmode_putstr_at)(char*,int,int);
}vmode_t;

#define k_puts( str ) \
        vmode_current->vmode_putstr( str )

#define k_puts_at(str, x, y) \
        vmode_current->vmode_putstr_at( str, x, y )
extern vmode_t *vmode_current;

void vga_init( vmode_t *new_vmode );
