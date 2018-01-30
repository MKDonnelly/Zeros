#include <portio.h>
#include <timing.h>
#include <serial/serial.h>
#include <cmos.h>
#include <pic.h>
#include <vga3h/vga3h.h>
#include <vga13h/vga13h.h>
#include <keyboard.h>

#include <string.h>
#include <bcd.h>
#include <types.h>
#include <bitwise.h>
#include <delay.h>
#include <debug.h>

#include <cpu.h>
#include <isr.h>
#include <idt.h>
#include <gdt.h>
#include <paging.h>
#include <kmalloc.h>
#include <multiboot.h>
#include <thread.h>
#include <sched.h>

#include <modeset.h>
#include <vgacommon.h>
#include <vgafont.h>
#include <bootdemo.h>
#include <shell.h>

#include <fs.h>
#include <initrd.h>

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
int startShell = 1;

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

  k_newline();
  k_newline();
  //k_printf("Enter some text: ");

  init_heap();
  init_paging();
  enable_ints();

  //demo();


  fs_root = init_initrd( h->mods->start );

  //add_thread( k_create_thread( demo, NULL, NULL, 0x3000, 0) );
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
