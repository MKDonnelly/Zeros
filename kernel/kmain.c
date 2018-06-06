#include <arch/current_arch>

#include <lib/keyboard.h>
#include <lib/string.h>
#include <lib/types.h>
#include <lib/bitwise.h>
#include <lib/timing.h>
#include <lib/generic_ll.h>
#include <lib/elf32.h>

#include <drivers/ata/ata_pio.h>

#include <kernel/multiboot.h>
#include <kernel/task.h>

#include <kernel/mm/heap_blocklist.h>
#include <kernel/mm/heap_bitmap.h>
#include <kernel/mm/heap.h>

#include <kernel/sched/sched.h>
#include <kernel/sched/round_robin.h>
#include <kernel/syscall.h>
#include <kernel/sched/workqueue.h>

#include <fs/fs.h>
#include <fs/initrd/initrd.h>
#include <fs/mbr.h>
#include <fs/zsfs/zsfs.h>
#include <lib/assert.h>

#include <drivers/drive.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_map.h>
#include <drivers/net/rtl8139.h>

void thread1(){
   int t1count = 0;
   while(t1count < 5){
      k_puts_at("1", t1count++, 0);
      for(int i = 0; i < 50000000; i++);
   }
}

void thread2(){
   int t2count = 0;
   while(t2count < 5){
      k_puts_at("2", t2count++, 1);
      for(int i = 0; i < 50000000; i++);
   }
}

//Defined in linker script
extern unsigned int ldscript_text_start;
extern unsigned int ldscript_text_end;
extern unsigned int ldscript_kernel_end;
extern unsigned int ldscript_initrd_start;


void kmain(struct multiboot_info *multiboot_info){

   arch_system_init();
   arch_timer_init( timing_main_handler );
   arch_keyboard_init( keyboard_main_handler );
   k_puts("\\c"); //clear screen
   
   //Have the heap start just after the text segment on an aligned boundary
   //Add 0x2000 since the kernel stack starts there.
   uint32_t heap_start = ALIGN_ON((int)&ldscript_kernel_end, 
                                  ARCH_PAGE_SIZE);
   //Keep in mind that only the first 4M of memory at the start of the
   //kernel is mapped in. If the size is changed to be larger, this 
   //may cause a page fault.
   heap_create( &global_kernel_heap, heap_start, 0x200000, 
                &bitmap_heap);
   
   //Copy the multiboot header since we will not be able to access
   //it once paging is setup
   struct multiboot_info *mb_copy = k_malloc(sizeof(struct multiboot_info), 0);
   memcpy( mb_copy, multiboot_info, sizeof(struct multiboot_info));

   //Also copy over the command line arguments
   char *cmdline = k_malloc(strlen((char*)multiboot_info->cmdline), 0);
   strcpy( cmdline, (char*)multiboot_info->cmdline);
   k_printf("Cmdline: %s\n", cmdline);

   //Initilize vm subsystem. This requires the heap, so we cannot
   //have arch_system_init do this.
   vm_init();
   
   current_scheduler = &rr_scheduler;
   current_scheduler->scheduler_setup();
   syscalls_init();

   drive_t *ata_drive = ata_pio_create_drive(0, 1);
   char *buffer = k_malloc(512, 0);
   memset(buffer, 512, 0 );
   ata_drive->drive_read_lba(ata_drive, buffer, 0, 1);
   mbr_setup_parttable(ata_drive);   

//   zsfs_create(ata_drive, 1);
   superblock_t *zsb = zsfs_get_superblock(ata_drive, 1);
   k_printf("%d, %d, %d, %d, %d\n", zsb->block_size, zsb->total_blocks, zsb->freelist_block, zsb->fsentries_block, zsb->fsentries_count);

//   pci_enumerate();
//   rtl8139_test_send();


//Read in first file from initrd (it will contain a test binary)
   char *program_buf = k_malloc( 5000, 0);
   fs_root = init_initrd( &ldscript_initrd_start );
   fs_node_t *first = fs_root->readdir( fs_root, 0 );
   first->read( first, 0, first->length, program_buf);

   ktask_t *new_task = utask_from_elf(program_buf);
   
   current_scheduler->scheduler_add_task(new_task);

   current_scheduler->scheduler_start();

   while(1) arch_halt_cpu();
}


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
