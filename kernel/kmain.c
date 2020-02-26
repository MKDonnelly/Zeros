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


fs_node_t *root_fs = NULL;

int stdout_test(fs_node_t *self, int offset, int len, char *buffer){
   //self and offset are ignored
   for(int i = 0; i < len; i++){
      k_printf("%c", buffer[i]);
   }

   return len;
}

int stdin_test(fs_node_t *self, int offset, int len, char *buffer){
   //self and offset are ignored
   keyboard_request(len, buffer);
   return len;
}

fs_node_t stdin_fs = {
   .name = "stdin",
   .flags = 0,
   .type = 0,
   .fs = NULL,
   .inode = 0,
   .read = stdin_test,
   .close = NULL,
};

fs_node_t stdout_fs = {
   .name = "stdout",
   .flags = 0,
   .type = 0,
   .fs = NULL,
   .inode = 0,
   .write = stdout_test,
   .close = NULL,
};

void init_usrland_fds(ktask_t *usr_task){
   //initialize all fds to null
   memset(usr_task->open_fs, sizeof(fs_node_t*) * 20, 0);

   usr_task->open_fs[0] = &stdin_fs;
   usr_task->open_fs[1] = &stdout_fs;
}


//Syscall
ktask_t *sys_spawn(char *binary, char *args, int stdinfd, int stdoutfd){
   //Read in binary
   fs_node_t *bin = zsfs_open(root_fs, binary);
   if( bin == NULL ){
      return NULL;
   }
   int bin_len = bin->len(bin);

   char *buf = k_malloc(bin_len, 0);
   bin->read(bin, 0, bin_len, buf);

   ktask_t *new_task = utask_from_elf(buf, 0);

   ktask_t *ctask = current_scheduler->scheduler_current_ktask();
   char *pwd = k_malloc(100, 0);
   strcpy(pwd, ctask->pwd);
   new_task->pwd = pwd;

   if( args != NULL ){
      char *arguments = k_malloc(100, 0);
      memset(arguments, 100, 0);
      memcpy(arguments, args, strlen(args)); 
      new_task->args = arguments;
   }else{
      new_task->args = NULL;
   }
   
   memset(new_task->open_fs, sizeof(fs_node_t*)*20, 0);   
   new_task->open_fs[0] = ctask->open_fs[stdinfd];
   new_task->open_fs[1] = ctask->open_fs[stdoutfd];
   current_scheduler->scheduler_add_task(new_task);
   //k_free(buf);

   return new_task;
}


void initrd_to_zsfs(initrd_sb_t *initrd, fs_node_t *zsfs_root, char *name){
  
   char path[100];
   memset(path, 100, 0);
   strcpy(path, "/");
   strcat(path, name);

   k_printf("Copying over %s\n", path);
 
   //TODO create function initrd_to_zsfs
   int len = initrd_sizeof(initrd, name);
   char *buf = k_malloc(len, 0);
   int read_bytes = initrd_read(initrd, name, len, buf);

   fs_node_t *f = zsfs_open(zsfs_root, path);
   if( f == NULL ){
      //File does not exist, so create it
      zsfs_create_file(zsfs_root, name);
      f = zsfs_open(zsfs_root, path);
   }
   f->write(f, 0, len, buf);

   k_free(buf);
}


void kmain(struct multiboot_info *multiboot_info){

   arch_system_init();
   arch_timer_init( timing_main_handler );
   arch_keyboard_init( keyboard_main_handler );
   k_puts("\\c"); //clear screen
 
   //Setup the heap on an aligned address after the end of the kernel
   //image. the ldscript already aligned the address.
   heap_create(&global_kernel_heap,(size_t)&ldscript_kernel_end, 0x300000, 
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
   k_printf("zsfs %x\n", zsfs);
   root_fs = zsfs_get_root(zsfs);
   k_printf("root_fs %x\n", root_fs);

   if( initrd != NULL ){
      //Copy over programs from initrd to zsfs
      initrd_to_zsfs(initrd, root_fs, "echo");
      initrd_to_zsfs(initrd, root_fs, "pwd");
      initrd_to_zsfs(initrd, root_fs, "rinput");
      initrd_to_zsfs(initrd, root_fs, "sar");
      initrd_to_zsfs(initrd, root_fs, "mkfile");
      initrd_to_zsfs(initrd, root_fs, "ls");
      initrd_to_zsfs(initrd, root_fs, "touch");
      initrd_to_zsfs(initrd, root_fs, "mkdir");
      initrd_to_zsfs(initrd, root_fs, "cat");
      initrd_to_zsfs(initrd, root_fs, "rm");
      initrd_to_zsfs(initrd, root_fs, "mydata");

      //Read in shell from initrd
      int len = initrd_sizeof(initrd, "shell");
      k_printf("Allocating %d bytes for shell binary...\n", len);
      char *buf = k_malloc(len, 0);
      initrd_read(initrd, "shell", len, buf);

      //Start the shell
      k_printf("Starting userland shell\n");
      ktask_t *shell = utask_from_elf(buf, 0);
      char *pwd = k_malloc(100, 0);
      strcpy(pwd, "/");
      shell->pwd = pwd;
      init_usrland_fds(shell);
      current_scheduler->scheduler_add_task(shell);
      current_scheduler->scheduler_start();
   }

   //Should never reach here.
   while(1) arch_halt_cpu();
}
