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

   //TODO return the number of bytes written
   return 0;
}

int stdin_test(fs_node_t *self, int offset, int len, char *buffer){
   k_printf("In stdin_test\n");
   keyboard_request(len, buffer);

   //TODO return the number of bytes read
   return 0;
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

   zsfs_register();
   //initrd_init((size_t*)&ldscript_initrd_start);
   initrd_sb_t *initrd = (initrd_sb_t*)&ldscript_initrd_start;
   k_printf("Initrd at %x\n", &ldscript_initrd_start);
   ata_enumerate();

//   zsfs_create(blkdev_find(0));

   fstype_t *zsfs = fsmanager_find_id(0x1234ABCD);
   k_printf("%x\n", zsfs);
   root_fs = zsfs_get_root(zsfs);
   k_printf("%x\n", root_fs);

   fs_node_t *f = root_fs->finddir(root_fs, "test.txt");
//   char data[] = "Hello, World!";
//   f->write(f, 0, strlen(data), data);
   char temp[20];
   f->read(f, 0, 14, temp);
   k_printf("Read %s\n", temp);
   k_printf("Len is %d\n", f->len(f));
   k_printf("file is %x\n", f);

//   dirent_t *d = zsfs_readdir(root_fs, 1);
//   k_printf("File is %s\n", d->name);
//   create_file(root_fs, "test.txt");
//   zsfs_create_dir(root_fs, "testdir"); 
   

/*
   if( initrd != NULL ){
      char *buf = k_malloc(20000, 0);
      int bytes_read = initrd_read(initrd, "test", 20000, buf);

      //Starting userland program
      k_printf("Starting userland program\n");
      ktask_t *new_task = utask_from_elf(buf);
      init_usrland_fds(new_task);
      current_scheduler->scheduler_add_task(new_task);
      current_scheduler->scheduler_start();
   }
*/

   while(1) arch_halt_cpu();
}
