
#pragma once

//A structure describing a module
//passed by grub
struct module{
   void *start;
   void *end;
   char *args;
   uint32_t not_used;
};

//Based on sample multiboot.h
//provided in the grub manual
struct multiboot_info{

   //Multiboot version
   uint32_t flags;

   //Upper and lower amounts
   //of memory, in KB
   uint32_t mem_lower;
   uint32_t mem_upper;

   //The root partition
   uint32_t boot_part;

   //Command line args
   char *cmdline;

   //Boot module list
   uint32_t num_mods;
   struct module *mods;
   //uint32_t mods_addr;
};

