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

#include "../lib/string.h"
#include "../lib/bcd.h"

#include "../cpu/cpu.h"
#include "../cpu/types.h"
#include "../cpu/isr.h"
#include "../cpu/idt.h"


void kmain(){

  //int var = 0;
  //double a = 1 / var;

  install_isrs();
  __asm__ __volatile__("int $2");
  //stop_cpu();
  while(1);

}

