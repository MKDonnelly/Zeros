#include <arch/x86/drivers/vgacommon/vgacommon.h>

vmode_t *vmode_current;
void vga_init(vmode_t *video_mode){
   vmode_current = video_mode;
}
