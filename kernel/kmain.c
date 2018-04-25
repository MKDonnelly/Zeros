#include <arch/x86/archx86.h>

#include <lib/keyboard.h>
#include <lib/string.h>
#include <lib/types.h>
#include <lib/bitwise.h>
#include <lib/debug.h>
#include <lib/timing.h>
#include <lib/abstract_ll.h>
#include <lib/elf.h>

#include <drivers/ata/ata_pio.h>

#include <kernel/multiboot.h>
#include <kernel/task.h>

#include <kernel/mm/heap_blocklist.h>
#include <kernel/mm/heap.h>

#include <kernel/sched/round_robin.h>

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

void kmain(struct multiboot_info *multiboot_info){

   arch_init_system();
   arch_timer_init( timing_main_handler );
   arch_keyboard_init( keyboard_main_handler );
   k_clear_screen();

   create_heap( &kernel_heap, KERNEL_VADDR+0x300000, 0x200000, 
                blocklist_malloc, blocklist_free, blocklist_init_heap );

   init_paging();

   setup_sched(rr_schedule, 100);
   rr_setup_scheduler();

   char *s1 = k_malloc(kernel_heap, 1024, 0x1000);
   rr_add_task( k_create_ktask( thread1, NULL, NULL, STACK_HEAD(s1, 1024)));
   char *s2 = k_malloc(kernel_heap, 1024, 0x1000);
   rr_add_task( k_create_ktask( thread2, NULL, NULL, STACK_HEAD(s2, 1024)));

   rr_start_scheduler();

/*
   char *userland_copy = (char*)k_malloc( kernel_heap, 5000, 0 );
   //Read program from initrd
   fs_root = init_initrd( ((struct module*)multiboot_info->mods)->start );
   fs_node_t *first = fs_root->readdir( fs_root, 0 );
   first->read( first, 0, first->length, userland_copy );

   struct elf_header *elf_hdr = (struct elf_header*)userland_copy;
   struct elf_prog_header *prog_hdr = (struct elf_prog_header*)(userland_copy + elf_hdr->e_phoff );
   for(int i = 0; i < elf_hdr->e_phnum; i++){
     prog_hdr = (struct elf_prog_header*)(userland_copy + elf_hdr->e_phoff + i * elf_hdr->e_phentsize);
     copy_to_physical( userland_copy + prog_hdr->p_offset, 
                       0x600000,
                       prog_hdr->p_filesize);
     k_printf("%x %x %d\n", prog_hdr->p_vaddr, userland_copy + prog_hdr->p_offset, prog_hdr->p_filesize);
   }
   char *ustack = k_malloc(kernel_heap, 0x1000, 0);
   pd_t *userland_pd = clone_pd( kernel_page_dir );
   map_page( 0x1000, 0x600000, userland_pd );
*/

/*
  init_syscalls();
  register_syscall( k_putchar, 0 );
  
  char *userland_copy = (char*)k_malloc( kernel_heap, 5000, 0 );

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

  char *ustack = k_malloc(kernel_heap, 0x1000, 0);
  pd_t *userland_pd = clone_pd( kernel_page_dir );
  map_page( 0x1000, 0x600000, userland_pd );

  init_scheduler( &rr_alg );
  register_syscall( current_sched_alg->exit_task, 1 );

  k_add_task( k_create_userland_task( (void*)0x1000, NULL, NULL, 0x1000, (uint32_t)ustack, userland_pd ));
*/
  while(1) arch_halt_cpu();
}

/*
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
}*/

/*
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
*/
