#include <arch/x86/drivers/vga3h/vga3h.h>
#include <arch/x86/drivers/vgacommon/vgacommon.h>

void kmain64(){
   vga_init( &vmode_vga3h );
   char *ptr = (char*)0xb8000;
   *(ptr) = 'Y';
   *(ptr + 2) = 'e';
   *(ptr + 4) = 'l';
   *(ptr + 6) = 'l';
   *(ptr + 8) = 'o';
}

