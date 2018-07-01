#include <arch/x86/archx86.h>

#include <arch/x86/pmode/gdt.h>
#include <arch/x86/drivers/pic.h>

void arch_system_init(int video_mode){
   arch_disable_ints();

   //Initilize descriptors
   gdt_init();
   interrupts_init();

   //Setup the PIC
   pic_init();

   //Setup the function pointers to use
   //the screen and 
   vga_init( &vmode_vga3h, KERNEL_VBASE );
}
