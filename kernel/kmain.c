#include <arch/current_arch>

#include <lib/keyboard.h>
#include <lib/string.h>
#include <lib/types.h>
#include <lib/bitwise.h>
#include <lib/timing.h>
#include <lib/genericll.h>
#include <lib/elf32.h>

#include <drivers/ata/ata_pio.h>

#include <kernel/multiboot.h>
#include <kernel/task.h>

#include <kernel/mm/blocklist_heap.h>
#include <kernel/mm/bitmap_heap.h>
#include <kernel/mm/heap.h>

#include <kernel/sched/sched.h>
#include <kernel/sched/round_robin.h>
#include <kernel/syscall.h>
#include <kernel/sched/workqueue.h>

#include <fs/vfs/fsmanager.h>
#include <fs/initrd/initrd.h>
#include <fs/zsfs/zsfs.h>
#include <fs/mbr.h>
#include <lib/assert.h>
#include <kernel/blkdev/blkdev.h>

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


int stdout_test(fs_node_t *self, int offset, int len, char *buffer){
   //self and offset are ignored
   for(int i = 0; i < len; i++){
      k_printf("%c", buffer[i]);
   }
}

int stdin_test(fs_node_t *self, int offset, int len, char *buffer){
   //offset is ignored right now
   for(int i = 0; i < len; i++){
      int key = keyboard_getchar();
      while( key == -1 ){
         current_scheduler->scheduler_yield_task();
         key = keyboard_getchar();
      }
      buffer[i] = (char)key;
   }
   k_printf("Returning with %c%c\n", buffer[0], buffer[1]);
}

fs_node_t stdin_fs = {
   .name = "stdin",
   .flags = 0,
   .type = 0,
   .fs = NULL,
   .inode = 0,
   .read = stdin_test,
};

fs_node_t stdout_fs = {
   .name = "stdout",
   .flags = 0,
   .type = 0,
   .fs = NULL,
   .inode = 0,
   .write = stdout_test,
};

void init_usrland_fds(ktask_t *usr_task){
   usr_task->open_fs[0] = &stdin_fs;
   usr_task->open_fs[1] = &stdout_fs;

   //0 for stdin, 1 for stdout, 2 for stderr, 
   //3 is the next free
   usr_task->next_free_node = 3;
}


fs_node_t *root_fs = NULL;

void kmain(struct multiboot_info *multiboot_info){

   arch_system_init();
   arch_timer_init( timing_main_handler );
   arch_keyboard_init( keyboard_main_handler );
   k_puts("\\c"); //clear screen
 
   //Setup the heap on an aligned address after the end of the kernel
   //image. the ldscript already aligned the address.
   heap_create(&global_kernel_heap,(size_t)&ldscript_kernel_end, 0x200000, 
                &bitmap_heap);
   
   //Copy the multiboot header since we will not be able to access
   //it once paging is setup
   multiboot_info_t *mb_copy = k_malloc(sizeof(multiboot_info_t), 0);
   memcpy( mb_copy, multiboot_info, sizeof(multiboot_info));

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

   zsfs_init();
   initrd_init(&ldscript_initrd_start);
   k_printf("Initrd at %x\n", &ldscript_initrd_start);
   ata_enumerate();

   fstype_t *initrd = fsmanager_find_id(0x11111111);
   if( initrd != NULL ){
      root_fs = &initrd->root_dir;
      dirent_t *first = root_fs->readdir(root_fs, 0);
      k_printf("Name is %s, %d\n", first->name, first->inode);

      fs_node_t *file = root_fs->finddir(root_fs, first->name); 
      k_free(first);
      char *buf = k_malloc(2000, 0);
      file->read( file, 0, file->len(file), buf );

      ktask_t *new_task = utask_from_elf(buf);
      init_usrland_fds(new_task);
      current_scheduler->scheduler_add_task(new_task);
      current_scheduler->scheduler_start();
   }

   while(1) arch_halt_cpu();
}
