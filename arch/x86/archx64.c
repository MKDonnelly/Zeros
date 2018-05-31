#include <arch/x86/archx64.h>
#include <arch/x86/drivers/vga3h/vga3h.h>

void arch_init_system(){
   vga_init( &vmode_vga3h );

   
}
