asm("jmp kmain"); //The bootsector immedietelly jumps to the
                  //start of this file and begins executing. This is
		  //undesirable if there is a function above kmain.
		  //add this so that kmain is always called.

#include "libprint.h"
#include "libvgaentry.h"

void kmain(){
   k_putchar('H');
   k_putchar('E');
   k_putchar('L');
   k_newline();
   k_putchar('L');
   k_putchar('O');
}

