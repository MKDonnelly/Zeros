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

   ktask_t *new_thread = k_create_task( test_function, (void*)20, task_exit, 0x1000, kernel_page_dir );
   k_add_task( new_thread );

   void *val = k_join_task( new_thread );

   k_printf("Thread joined with %d\n", (int)val);

   k_exit_task( (void*)0 );
}

extern void arch_userland_jump();
#include <staging/syscall.h>

void userland_thread(){
   syscall_putchar('Z'); 
   while(1);
}

void kmain(struct multiboot_info *multiboot_info){

  //Arch initilization
  arch_init_system();
  arch_disable_ints();
  arch_timer_init( timing_main_handler );
  arch_keyboard_init( keyboard_main_handler );

  k_clear_screen();


  //See heap.h for kernel_heap
  create_heap( &kernel_heap, 0x300000, 0x200000, blocklist_malloc, blocklist_free, blocklist_init_heap );


  //reserve space for syscall stack
  char *syscall_stack = (char*)k_malloc( kernel_heap, 0x1000, 0, 0 );

  init_syscalls();
  set_kernel_stack( (uint32_t)syscall_stack );

  page_directory_t *userland_dir = clone_dir( kernel_ref_dir );
  
  uint32_t userland_prog = 0x600000;

  //Read program from initrd
  fs_root = init_initrd( ((struct module*)multiboot_info->mods)->start );
  fs_node_t *first = fs_root->readdir( fs_root, 0 );
  k_printf("%s: %d\n", first->name, first->length);
  first->read( first, 0, first->length, (char*)userland_prog );  

  void (*f)() = (void*)userland_prog;
  //f();

  init_paging();  


  setup_scheduler( &rr_alg );

  k_add_task( k_create_userland_task( f, NULL, NULL, 0x1000, kernel_page_dir ) );

  //k_add_task( k_create_task( main_kernel_thread, NULL, NULL, 0x4000, kernel_page_dir ) );


//  k_add_task( k_create_task( thread1, NULL, NULL, 0x1000, kernel_page_dir) );  
//  k_add_task( k_create_task( thread2, NULL, NULL, 0x1000, kernel_page_dir) );  
//  k_add_task( k_create_task( thread3, NULL, NULL, 0x1000, kernel_page_dir) );  

  k_add_task( k_create_task( thread4, NULL, NULL, 0x1000, kernel_page_dir) );  
/*  k_add_task( k_create_task( thread5, NULL, NULL, 0x1000, kernel_page_dir) );
  k_add_task( k_create_task( thread6, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_task( thread7, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_task( thread8, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_task( thread9, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_task( threada, NULL, NULL, 0x1000, kernel_page_dir) );  
  */
  start_scheduler();
  while(1);
}
