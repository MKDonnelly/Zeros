asm("jmp kmain"); //The bootsector immedietelly jumps to the
                  //start of this file and begins executing. This is
		  //undesirable if there is a function above kmain.
		  //add this so that kmain is always called.

#include "../drivers/portio.h"
#include "../drivers/timing.h"
#include "../drivers/serial/serial.h"
#include "../drivers/cmos.h"
#include "../drivers/pic.h"
#include "../drivers/vgatext/vgatext.h"
#include "../drivers/vga13h/vga13hmode.h"
#include "../drivers/keyboard.h"

#include "../lib/string.h"
#include "../lib/bcd.h"
#include "../lib/types.h"
#include "../lib/keyboard_map.h"
#include "../lib/bitwise.h"

#include "../cpu/cpu.h"
#include "../cpu/isr.h"
#include "../cpu/idt.h"

#include "kmalloc.h"
#include "paging.h"

void kmain(){

  //Initilize the PIC
  remap_pic();
  //Create the IDT and initilize
  //the interrupt handlers
  install_interrupts();

  init_keyboard();
  move_cursorl( k_xy_to_linear( 0, 0 ) );

  //Make sure to enable 
  //the interrupts
  enable_ints();

  while(1);
}

//         Examples

/*       Dynamic memory allocation
  char *mystr = (char*)kmalloc(100, 0);
  strcpy( mystr, "Testing..." );
  k_print( mystr );
*/

/*       Using the keyboard
  //Initilize the PIC
  remap_pic();
  //Create the IDT and initilize
  //the interrupt handlers
  install_interrupts();
  
  init_keyboard();
  move_cursorl( k_xy_to_linear( 0, 0 ) );

  //Make sure to enable 
  //the interrupts
  //enable_ints();
  asm volatile("sti");
  while(1);
*/