#include <arch/x86/archx86.h>

#include <arch/x86/pmode/gdt.h>
#include <arch/x86/drivers/pic.h>

void arch_system_init(int video_mode){

   //Initilize descriptors
   gdt_init();
   interrupts_init();

   //Re-map the pic
   pic_init();

   //Setup the function points to use
   //the screen
   vga_init(0);

   arch_disable_ints();
}
