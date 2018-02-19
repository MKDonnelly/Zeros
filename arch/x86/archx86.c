#include <arch/x86/archx86.h>

void arch_init_system(int video_mode){

   //Initilize descriptors
   init_gdt();
   init_interrupts();

   //Re-map the pic
   remap_pic();

   //Setup the function points to use
   //the screen
   init_vga(0);
}
