asm("jmp kmain"); //The bootsector immedietelly jumps to the
                  //start of this file and begins executing. This is
		  //undesirable if there is a function above kmain.
		  //add this so that kmain is always called.

#include "../drivers/portio.h"
#include "../drivers/timing.h"
#include "../drivers/vgatext/console.h"
#include "../drivers/serial/serial.h"
#include "../drivers/cmos.h"
#include "../drivers/vgatext/print.h"
#include "../drivers/pic.h"

#include "../lib/string.h"
#include "../lib/bcd.h"

#include "../cpu/cpu.h"
#include "../cpu/types.h"
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
  enable_ints();

  k_print("Press a key: " );

  //__asm__ __volatile__("int $33");
  //stop_cpu();
  while(1);

}

