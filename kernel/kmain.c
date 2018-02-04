#include <arch/x86/drivers/timer.h>
#include <arch/x86/drivers/serial/serial.h>
#include <arch/x86/drivers/cmos.h>
#include <arch/x86/drivers/pic.h>
#include <arch/x86/drivers/vga3h/vga3h.h>
//#include <arch/x86/drivers/keyboard.h>
#include <arch/x86/drivers/vga13h/vga13h.h>
#include <arch/x86/drivers/vgacommon/modeset.h>
#include <arch/x86/drivers/vgacommon/vgacommon.h>
#include <arch/x86/drivers/vgacommon/vgafont.h>

#include <arch/x86/portio.h>
#include <arch/x86/cpu.h>
#include <arch/x86/paging.h>

#include <lib/string.h>
#include <lib/bcd.h>
#include <lib/types.h>
#include <lib/bitwise.h>
#include <lib/delay.h>
#include <lib/debug.h>
#include <lib/bootdemo.h>
#include <lib/shell.h>
#include <lib/timing.h>

#include <kernel/kmalloc.h>
#include <kernel/multiboot.h>
#include <kernel/thread.h>
#include <kernel/sched.h>


#include <fs/fs.h>
#include <fs/initrd/initrd.h>

//Horrible hack to start the shell
//once the boot demo is done.
int startShell = 0;

void myf(){
   k_printf("In my function!\n");
}

void kbh(char c){
   k_printf("Keyboard handler!");
}

#include <arch/x86/archx86.h>
#include <lib/keyboard.h>

void kmain(struct multiboot_info *h){

  //set_vga_mode( vga_3h_mode );
  //write_font( g_8x16_font, 16 );

  //Arch initilization
  arch_init_system();
  arch_timer_init( timing_main_handler );
  arch_keyboard_init( keyboard_main_handler );

  //Timer subsystem initilization
  timing_set_alarm( myf, 1000 );

  keyboard_register_key_callback( kbh, 'a' );

  k_clear_screen();
  init_heap();
  init_paging();
  enable_ints();

/*
  fs_root = init_initrd( h->mods->start );
  
  add_thread( k_create_thread( demo, NULL, NULL, 0x3000, 0) );
  add_thread( k_create_thread( zeros_shell, NULL, NULL, 4192, 0) );
*/
  
  //init_threading();
  while(1);
}
