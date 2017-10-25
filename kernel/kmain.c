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
  install_isrs();
  //Enable irq1 (keyboard)
  enable_irq1();
  //Make sure to enable 
  //the interrupts
  //enable_ints();
  __asm__ __volatile__("sti");


  move_cursorl( 0 );
  k_print("Enter some text: " );

  //__asm__ __volatile__("int $35");
  //stop_cpu();
  while(1);

}

