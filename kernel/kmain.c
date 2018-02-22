#include <arch/x86/archx86.h>

#include <lib/keyboard.h>
#include <lib/string.h>
#include <lib/bcd.h>
#include <lib/types.h>
#include <lib/bitwise.h>
#include <lib/delay.h>
#include <lib/debug.h>
#include <lib/bootdemo.h>
#include <lib/shell.h>
#include <lib/timing.h>
#include <lib/abstract_ll.h>

#include <kernel/kmalloc.h>
#include <kernel/multiboot.h>
#include <kernel/thread.h>
#include <kernel/sched.h>

#include <fs/fs.h>
#include <fs/initrd/initrd.h>

//Horrible hack to start the shell
//once the boot demo is done.
int startShell = 1;

void thread1(){
   int t1count = 0;
   while(1){
      k_printf_at("1", t1count++, 0);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread2(){
   int t2count = 0;
   while(1){
      k_printf_at("2", t2count++, 1);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread3(){
   int t3count = 0;
   while(1){
      k_printf_at("3", t3count++, 2);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread4(){
   int t4count = 0;
   while(1){
      k_printf_at("4", t4count++, 3);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread5(){
   int t5count = 0;
   while(1){
      k_printf_at("5", t5count++, 4);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread6(){
   int t6count = 0;
   while(1){
      k_printf_at("6", t6count++, 5);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread7(){
   int t7count = 0;
   while(1){
      k_printf_at("7", t7count++, 6);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread8(){
   int t8count = 0;
   while(1){
      k_printf_at("8", t8count++, 7);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread9(){
   int t9count = 0;
   while(1){
      k_printf_at("9", t9count++, 8);
      for(int i = 0; i < 50000000; i++);
   }
}

void threada(){
   int tacount = 0;
   while(1){
      k_printf_at("a", tacount++, 9);
      for(int i = 0; i < 50000000; i++);
   }
}

void test_function(void *arg){
   k_printf("In test function with %d!\n", (int)arg);
   thread_exit( (void*)99 );
}


void main_kernel_thread(){

   k_printf("In main kernel thread\n");

   kthread_t *new_thread = k_create_thread( test_function, (void*)20, thread_exit, 0x1000 );
   add_thread( new_thread );

   void *val = thread_join( new_thread );

   k_printf("Thread joined with %d\n", (int)val);

/*
   while(1){
      if( portb_read( 0x64 ) & 0x20 ){
         k_printf("Mouse with data %x %x %x\n", portb_read( 0x60 ), portb_read(0x60), portb_read(0x60));
         for(int i = 0; i < 10000000; i++);
      }
   }*/
   thread_exit( 0 );
}

void kbh(char c){
   k_printf("Key pressed\n");
}

#include <staging/heap_blocklist.h>
#include <staging/heap.h>

void kmain(struct multiboot_info *multiboot_info){

  //set_vga_mode( vga_3h_mode );
  //write_font( g_8x16_font, 16 );

  //Arch initilization
  arch_init_system();
  arch_disable_ints();
  arch_timer_init( timing_main_handler );
  arch_keyboard_init( keyboard_main_handler );

  //Keyboard system initilization
  keyboard_register_key_callback( kbh, 'a' );

  k_clear_screen();

  init_heap();
  //See heap.h for kernel_heap
  create_heap( &kernel_heap, 0x300000, 0xf0000, blocklist_malloc, blocklist_free, blocklist_init_heap );



//  char *mem = (char*)kernel_heap.malloc( &kernel_heap, 100, 0x1000, 0 );
//  k_printf("Allocated memory at %x\n", mem );

//  k_printf("%x %d %d\n", kernel_heap.start, kernel_heap.len, kernel_heap.size_left);


  init_paging();

  //Timer subsystem initilization
  //Required dynamic memory
  timing_set_alarm( schedule, 100 );

  //Serial port test
  sp_init();
  sp_putstr("Hello on serial!\n");

//TODO map in initrd with paging
//  fs_root = init_initrd( h->mods->start );

//  add_thread( k_create_thread( main_kernel_thread, NULL, thread_exit, 0x4000 ) );


  add_thread( k_create_thread( thread1, NULL, NULL, 0x1000) );  
  add_thread( k_create_thread( thread2, NULL, NULL, 0x1000) );  
  add_thread( k_create_thread( thread3, NULL, NULL, 0x1000) );  
  add_thread( k_create_thread( thread4, NULL, NULL, 0x1000) );  
  add_thread( k_create_thread( thread5, NULL, NULL, 0x1000) );  
  add_thread( k_create_thread( thread6, NULL, NULL, 0x1000) );  
  add_thread( k_create_thread( thread7, NULL, NULL, 0x1000) );  
  add_thread( k_create_thread( thread8, NULL, NULL, 0x1000) );  
  add_thread( k_create_thread( thread9, NULL, NULL, 0x1000) );  
  add_thread( k_create_thread( threada, NULL, NULL, 0x1000) );  

  
  init_threading();
  while(1);
}
