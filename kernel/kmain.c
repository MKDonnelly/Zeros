#include <arch/x86/archx86.h>

#include <lib/keyboard.h>
#include <lib/string.h>
#include <lib/types.h>
#include <lib/bitwise.h>
#include <lib/debug.h>
#include <lib/timing.h>
#include <lib/generic_ll.h>
#include <lib/elf.h>

#include <drivers/ata/ata_pio.h>

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

#include <kernel/mm/heap_bitmap.h>

void kmain(struct multiboot_info *multiboot_info){

   arch_init_system();
   arch_timer_init( timing_main_handler );
   arch_keyboard_init( keyboard_main_handler );
   k_clear_screen();


   create_heap( &global_kernel_heap, 0x300000+0xC0000000, 0x200000, 
                &bitmap_heap );

  k_printf("Working");

  init_syscalls();
  register_syscall( k_putchar, 0 );
  
  char *userland_copy = (char*)k_malloc( 5000, 0 );

  //Read program from initrd
  fs_root = init_initrd( ((struct module*)multiboot_info->mods)->start );
  k_printf("Initrd at %x\n", ((struct module*)multiboot_info->mods)->start);
  fs_node_t *first = fs_root->readdir( fs_root, 0 );
  first->read( first, 0, first->length, userland_copy );

  //elf file now in userland_copy
  struct elf_header *elf_hdr = (struct elf_header*)userland_copy;
  struct elf_prog_header *prog_hdr = (struct elf_prog_header*)(userland_copy + elf_hdr->e_phoff );
 
  init_paging();  

  for(int i = 0; i < elf_hdr->e_phnum; i++){
    prog_hdr = (struct elf_prog_header*)(userland_copy + elf_hdr->e_phoff + i * elf_hdr->e_phentsize);
    copy_to_physical( userland_copy + prog_hdr->p_offset, 
                      0x600000,
                      prog_hdr->p_filesize);
    k_printf("%x %x %d\n", prog_hdr->p_vaddr, userland_copy + prog_hdr->p_offset, prog_hdr->p_filesize);
  }

  char *ustack = k_malloc( 0x1000, 0);
  pd_t *userland_pd = clone_pd( kernel_page_dir );
  map_page( 0x1000, 0x600000, userland_pd );

  init_scheduler( &rr_alg );
  register_syscall( current_sched_alg->exit_task, 1 );

  k_add_task( k_create_userland_task( (void*)0x1000, NULL, NULL, 0x1000, (uint32_t)ustack, userland_pd ));

/*
  k_add_task( k_create_kernel_task( thread1, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_kernel_task( thread2, NULL, NULL, 0x1000, kernel_page_dir) );  
 */
  start_scheduler();
  while(1) arch_halt_cpu();
}

/*
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

int y_offset = 0;
int x_offset = 0;

void m( mouse_packet_t p ){
//   k_clear_screen();
   //As we go down the screen, y increases.
   //as we go up the screen, y decreases.
   //But the delta_y is exactly the opposite:
   //negative values go down and positive values
   //go up, so negate delta_y.
   y_offset += (-p.delta_y) / 2;
   x_offset += p.delta_x / 2;
   k_printf_at("*", x_offset, y_offset);
}

uint16_t start_state = 0xACF1;
uint16_t lfsr = 0xACF1;
uint16_t bit;
int period = 0;

int random(){
   bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
   lfsr = (lfsr >> 1) | (bit << 15);
   ++period;
   return lfsr;
}*/


/*
void kmain(struct multiboot_info *multiboot_info){

  arch_init_system();
  arch_timer_init( timing_main_handler );
  arch_keyboard_init( keyboard_main_handler );
  k_clear_screen();


  //Disable irqs on ata
  portb_write( ATA_PIO_CTRL_P, 0x02 );

  ////////////////////
  uint16_t *identify_data = identify_drive(0, 0);
  uint32_t total_sectors = ( identify_data[61] << 16 ) | identify_data[60];
  k_printf("Total sectors: %d\n", total_sectors);

  //test write 

  uint16_t *data = k_malloc( kernel_heap, sizeof(uint16_t)*256, 0);
  memset( data, sizeof(uint16_t)*256, 7);
  uint8_t sectors = 1;
  uint32_t lba = 0;

  ata_pio_write_sector( sectors, lba, data );

  uint16_t *buf = ata_pio_read_sector( 1, 0 );
  for(int i = 0; i < 10; i++)
     k_printf("%d\n", (uint8_t)buf[i]);
  ///////////////////


  //mouse_init();
  //register_mouse_handler( m );
  //arch_enable_ints();
  init_syscalls();
  register_syscall( k_putchar, 0 );
  

  uint32_t userland_prog = 0x600000;
  uint32_t userland_stack = 0x610000;

  char *userland_copy = (char*)k_malloc( kernel_heap, 500, 0 );

  //Read program from initrd
  fs_root = init_initrd( ((struct module*)multiboot_info->mods)->start );
  k_printf("Initram at %x\n", ((struct module*)multiboot_info->mods)->start);
  arch_stop_cpu();
  fs_node_t *first = fs_root->readdir( fs_root, 0 );

  first->read( first, 0, first->length, userland_copy );

  //elf file now in userland_copy
  struct elf_header *elf_hdr = (struct elf_header*)userland_copy;
  struct elf_prog_header *prog_hdr = (struct elf_prog_header*)(userland_copy + elf_hdr->e_phoff );
  
  init_paging();  
  init_scheduler( &rr_alg );
//  char *ustack = (char*)k_malloc(kernel_heap, 0x1000, 0x1000);
//  k_add_task( k_create_userland_task( userland, NULL, NULL, 0x1000, (uint32_t)ustack, kernel_page_dir ));

  //pd_t *userland_dir = clone_pd( kernel_page_dir );
  //copy_to_physical( userland_copy, userland_prog, first->length );
  ///map_page( prog_hdr->p_vaddr, userland_prog, userland_dir );
  //map_page( userland_stack, userland_stack, userland_dir );
  //k_add_task( k_create_userland_task( (void*)elf_hdr->e_entry, NULL, NULL, 0x1000, userland_stack, userland_dir ) );

  page_directory_t *userland_dir = clone_page_dir( kernel_page_dir );
  copy_to_physical( userland_copy, first->length, userland_prog );
  map_page( prog_hdr->p_vaddr, userland_prog, userland_dir );
  map_page( userland_stack, userland_stack, userland_dir );
  k_add_task( k_create_userland_task( (void*)elf_hdr->e_entry, NULL, NULL, 0x1000, userland_stack, userland_dir ) );
*/
/*
  for(int i = 0; i < elf_hdr->e_phnum; i++){
    prog_hdr = (struct elf_prog_hreader*)(userland_copy + elf_hdr->e_phoff + i * elf_hdr->e_phentsize);
    copy_to_physical( userland_copy + prog_hdr->p_offset, 
                      prog_hdr->p_filesize,
                      userland_prog );
    k_printf("%x %x %d\n", prog_hdr->p_vaddr, userland_copy + prog_hdr->p_offset, prog_hdr->p_filesize);
  }
 


  k_add_task( k_create_kernel_task( thread1, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_kernel_task( thread2, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_kernel_task( thread3, NULL, NULL, 0x1000, kernel_page_dir) );  

  k_add_task( k_create_kernel_task( thread4, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_kernel_task( thread5, NULL, NULL, 0x1000, kernel_page_dir) );
  k_add_task( k_create_kernel_task( thread6, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_kernel_task( thread7, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_kernel_task( thread8, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_kernel_task( thread9, NULL, NULL, 0x1000, kernel_page_dir) );  
  k_add_task( k_create_kernel_task( threada, NULL, NULL, 0x1000, kernel_page_dir) );  
 
  start_scheduler();
  while(1) arch_halt_cpu();
}*/
