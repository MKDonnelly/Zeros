asm("jmp kmain"); //The bootsector immedietelly jumps to the
                  //start of this file and begins executing. This is
		  //undesirable if there is a function above kmain.
		  //add this so that kmain is always called.

#include "libprint.h"
extern char* VIDEO_MEMORY;
#include "libvgaentry.h"
#include "libportio.h"
#include "libcursor.h"
#include "libscroll.h"
#include "libscreencap.h"
#include "libtiming.h"

void kmain(){

  k_clear_screen();
  k_print("Testing..."); 
  delay_second(1);
  k_print("Delay?");

}

