#include <arch/x86/archx86.h>

#include <lib/keyboard.h>
#include <lib/string.h>
#include <lib/bcd.h>
#include <lib/types.h>
#include <lib/bitwise.h>
#include <lib/delay.h>
#include <lib/debug.h>
#include <lib/bootdemo.h>
#include <lib/timing.h>
#include <lib/abstract_ll.h>

#include <kernel/multiboot.h>
#include <kernel/task.h>

#include <kernel/mm/heap_blocklist.h>
#include <kernel/mm/heap.h>

#include <kernel/sched/round_robin.h>
#include <kernel/sched/sched.h>

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
   k_exit_task( (void*)99 );
}


void main_kernel_thread(){

   k_printf("In main kernel thread\n");

   while(1)
      arch_stop_cpu();
/*
   ktask_t *new_thread = k_create_task( test_function, (void*)20, task_exit, 0x1000, kernel_page_dir );
   k_add_task( new_thread );

   void *val = k_join_task( new_thread );

   k_printf("Thread joined with %d\n", (int)val);
*/
/*
   while(1){
      if( portb_read( 0x64 ) & 0x20 ){
         k_printf("Mouse with data %x %x %x\n", portb_read( 0x60 ), portb_read(0x60), portb_read(0x60));
         for(int i = 0; i < 10000000; i++);
      }
   }*/
   k_exit_task( (void*)0 );
}


void kmain(struct multiboot_info *multiboot_info){

  //set_vga_mode( vga_3h_mode );
  //write_font( g_8x16_font, 16 );

  //Arch initilization
  arch_init_system();
  arch_disable_ints();
  arch_timer_init( timing_main_handler );
  arch_keyboard_init( keyboard_main_handler );

  //Keyboard system initilization
  //keyboard_register_key_callback( kbh, 'a' );

  k_clear_screen();

  //See heap.h for kernel_heap
  create_heap( &kernel_heap, 0x300000, 0x200000, blocklist_malloc, blocklist_free, blocklist_init_heap );


/*           video memory test
  uint16_t *video_memory = (uint16_t*)multiboot_info->fb_addr;
  for(int i = 0; i < 65536 * 2; i++){
     video_memory[i] = i;
  }
  while(1);*/
 
  //Copy over initrd before paging gets in the way
  
  k_printf("Modules %d\n", multiboot_info->num_mods);
  k_printf("%x\n", ((struct module*)multiboot_info->mods)->start);
  k_printf("%x", ((struct module*)multiboot_info->mods)->end);

  int initrd_size = ((struct module*)multiboot_info->mods)->end - ((struct module*)multiboot_info->mods)->start;
  char *initrd = k_malloc(kernel_heap, initrd_size, 0, 0);
  memcpy( initrd, ((struct module*)multiboot_info->mods)->start, initrd_size);

  fs_root = init_initrd( initrd );
  fs_node_t *files = fs_root->readdir( fs_root, 1 );
  k_printf("%s\n", files->name );
  char *filebuf = k_malloc( kernel_heap, files->length, 0, 0);
  files->read( files, 0, files->length, filebuf );
  k_printf("%s\n", filebuf);
 
  //init_paging();



  setup_scheduler( &rr_alg );

//TODO map in initrd with paging
//  fs_root = init_initrd( h->mods->start );

//  k_add_task( k_create_task( main_kernel_thread, NULL, NULL, 0x4000, kernel_page_dir ) );

/*
  k_add_task( k_create_task( thread1, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_task( thread2, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_task( thread3, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_task( thread4, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_task( thread5, NULL, NULL, 0x1000, kernel_page_dir) );
  k_add_task( k_create_task( thread6, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_task( thread7, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_task( thread8, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_task( thread9, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_task( threada, NULL, NULL, 0x1000, kernel_page_dir) );  
*/  
  start_scheduler();
  
  while(1);
}
