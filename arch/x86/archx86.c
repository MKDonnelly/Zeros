#include <arch/x86/archx86.h>

#include <arch/x86/pmode/gdt.h>
#include <arch/x86/drivers/pic.h>

void arch_init_system(int video_mode){

   //Initilize descriptors
   init_gdt();
   init_interrupts();

   //Re-map the pic
   remap_pic();

   //Setup the function points to use
   //the screen
   init_vga(0);

   arch_disable_ints();
}
