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

#include "../lib/string.h"
#include "../lib/bcd.h"
#include "../lib/types.h"
#include "../lib/keyboard_map.h"

#include "../cpu/cpu.h"
#include "../cpu/isr.h"
#include "../cpu/idt.h"


void kmain(){

  //Initilize the PIC
  init_pic();
  //Create the IDT and initilize
  //the interrupt handlers
  install_interrupts();
  //Enable irq1 (keyboard)
  enable_irqs();
  //Make sure to enable 
  //the interrupts
  //enable_ints();
  asm volatile("sti");

  asm("int $32");
  asm("int $31");

  //move_cursorl( 0 );
  //k_print("Enter some text: " );
  while(1);
  //stop_cpu();
}

/* Setup the keyboard:
 * init_pic();
 * install_isrs();
 * enable_irqs();
 * asm volatile("sti");
 *
 * while(1);
 *
 * */

