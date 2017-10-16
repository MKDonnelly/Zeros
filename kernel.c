__asm__("jmp kmain");

#include "libprint.h"
#include "libtiming.h"
#include "libkeyboard.h"


void kmain(void)
{


  k_print("Hello from the kernel");
  delay_second(5);
  k_print("Another hello just to be sure");
  idt_init();
  kb_init();


  while(1) __asm__("hlt\n\t");
}
