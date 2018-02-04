#include <arch/x86/portio.h>
#include <arch/x86/drivers/timer.h>
#include <arch/x86/drivers/serial/serial.h>
#include <arch/x86/drivers/cmos.h>
#include <arch/x86/drivers/pic.h>
#include <arch/x86/drivers/vga3h/vga3h.h>
#include <arch/x86/drivers/vga13h/vga13h.h>
#include <arch/x86/cpu.h>
#include <arch/x86/isr.h>
#include <arch/x86/idt.h>
#include <arch/x86/gdt.h>
#include <arch/x86/paging.h>
#include <arch/x86/drivers/keyboard.h>

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

#include <arch/x86/drivers/vgacommon/modeset.h>
#include <arch/x86/drivers/vgacommon/vgacommon.h>
#include <arch/x86/drivers/vgacommon/vgafont.h>

#include <fs/fs.h>
#include <fs/initrd/initrd.h>

//Horrible hack to start the shell
//once the boot demo is done.
int startShell = 0;

void myf(){
   k_printf("In my function!\n");
}

#include <arch/x86/archx86.h>

void kmain(struct multiboot_info *h){

  //set_vga_mode( vga_3h_mode );
  //write_font( g_8x16_font, 16 );

  arch_init_system();
  k_clear_screen();
  arch_timer_init( timing_main_handler );
  timing_set_alarm( myf, 1000 );

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
