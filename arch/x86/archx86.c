#include <arch/x86/archx86.h>

#include <arch/x86/pmode/gdt.h>
#include <arch/x86/drivers/pic.h>

void arch_system_init(int video_mode){

   //Initilize descriptors
   gdt_init();
   interrupts_init();

   //Setup the PIC
   pic_init();

   //Setup the function points to use
   //the screen
   vga_init( &vmode_vga3h );   

   arch_disable_ints();
}
