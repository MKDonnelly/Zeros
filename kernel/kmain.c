#include <arch/x86/portio.h>
#include <arch/x86/timing.h>
#include <arch/x86/serial/serial.h>
#include <arch/x86/cmos.h>
#include <arch/x86/pic.h>
#include <arch/x86/vga3h/vga3h.h>
#include <arch/x86/vga13h/vga13h.h>
#include <arch/x86/cpu.h>
#include <arch/x86/isr.h>
#include <arch/x86/idt.h>
#include <arch/x86/gdt.h>
#include <arch/x86/paging.h>
#include <arch/x86/keyboard.h>

#include <lib/string.h>
#include <lib/bcd.h>
#include <lib/types.h>
#include <lib/bitwise.h>
#include <lib/delay.h>
#include <lib/debug.h>
#include <lib/bootdemo.h>
#include <lib/shell.h>

#include <kernel/kmalloc.h>
#include <kernel/multiboot.h>
#include <kernel/thread.h>
#include <kernel/sched.h>

#include <arch/x86/vgacommon/modeset.h>
#include <arch/x86/vgacommon/vgacommon.h>
#include <arch/x86/vgacommon/vgafont.h>

#include <fs/fs.h>
#include <fs/initrd/initrd.h>

void thread1(){

   int t1count = 0;
   while(1){
      k_printf_at("1", t1count++, 5);
      for(int i = 0; i < 50000000; i++);
   }  
}

void thread2(){
   int t2count = 0;
   while(1){
      k_printf_at("2", t2count++, 7);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread3(){
   int t3count = 0;
   while(1){
      k_printf_at("3", t3count++, 9);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread4(){
   int t4count = 0;
   while(1){
      k_printf_at("4", t4count++, 11);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread5(){
   int t5count = 0;
   while(1){
      k_printf_at("5", t5count++, 13);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread6(){
   int t6count = 0;
   while(1){
      k_printf_at("6", t6count++, 15);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread7(){
   int t7count = 0;
   while(1){
      k_printf_at("7", t7count++, 17);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread8(){
   int t8count = 0;
   while(1){
      k_printf_at("8", t8count++, 19);
      for(int i = 0; i < 50000000; i++);
   }
}

//Horrible hack to start the shell
//once the boot demo is done.
int startShell = 0;

void kmain(struct multiboot_info *h){

  //set_vga_mode( vga_3h_mode );
  //write_font( g_8x16_font, 16 );
  init_vga(0);
  k_clear_screen();

  init_gdt();
  remap_pic();

  init_interrupts();
  init_timer(0, 0, 0);
  init_keyboard();
  //disable_ints();
  //k_printf("Enter some text: ");

  k_newline();
  k_newline();

  init_heap();
  init_paging();
  enable_ints();

  fs_root = init_initrd( h->mods->start );
  
  add_thread( k_create_thread( demo, NULL, NULL, 0x3000, 0) );
  add_thread( k_create_thread( zeros_shell, NULL, NULL, 4192, 0) );

  
/*
  add_thread( k_create_thread( thread1, NULL, NULL, 0x1000, 0) );
  add_thread( k_create_thread( thread2, NULL, NULL, 0x1000, 0) );
  add_thread( k_create_thread( thread3, NULL, NULL, 0x1000, 0) );
  add_thread( k_create_thread( thread4, NULL, NULL, 0x1000, 0) );
  add_thread( k_create_thread( thread5, NULL, NULL, 0x1000, 0) );
  add_thread( k_create_thread( thread6, NULL, NULL, 0x1000, 0) );
  add_thread( k_create_thread( thread7, NULL, NULL, 0x1000, 0) );
  add_thread( k_create_thread( thread8, NULL, NULL, 0x1000, 0) );
*/  

  init_threading();
  while(1);
}
