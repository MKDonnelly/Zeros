#pragma once

#include <lib/types.h>

//Heavily based on the multiboot specification
//documentation at https://www.gnu.org/software/grub/manual/multiboot/

//This is what is ready by grub
typedef struct multiboot_header{
   uint32_t magic;
   uint32_t flags;
   uint32_t checksum;

   //Only valid when using AOUT_KLUDGE
   //i.e. we are loaded from an a.out format
   //executable (Zeros is not)
   uint32_t header_addr;
   uint32_t load_addr;
   uint32_t load_end_addr;
   uint32_t bss_end_addr;
   uint32_t entry_addr;

   //Only valid if flags for video mode is set
   uint32_t mode_type;
   uint32_t width;
   uint32_t height;
   uint32_t depth;
}multiboot_header_t;

//Symbol table for an a.out format
typedef struct mb_aout_symbol_table{
   uint32_t tabsize;
   uint32_t strsize;
   uint32_t addr;
   uint32_t reserved;
}mb_aout_symbol_table_t;

//Symbol table for ELF
typedef struct mb_elf_table{
   uint32_t num;
   uint32_t size;
   uint32_t addr;
   uint32_t shndx;
}mb_elf_table_t;


//A structure describing a module
//passed by grub
typedef struct mb_module{
   void *start;
   void *end;
   char *args;
   uint32_t not_used;
}mb_module_t;

//Masks for the flags member below
//so our kernel can tell what there is.

//Did grub pass information about memory?
#define MEMORY_INFO_PASSED    (1 << 0)

//Did grub pass information about a boot device?
#define BOOT_DEV_PASSED       (1 << 1)

//Is there anything in the command line?
#define CMD_LINE_PASSED       (1 << 2)

//Are there any modules?
#define MODULES_PASSED        (1 << 3)

//These are mutually exclusive: either
//we were booted with an ELF executable
//or something else
#define NONELF_SYMBOLS_PASSED (1 << 4)
#define ELF_SYMBOLS_PASSED    (1 << 5)

//Was there a memory map passed?
#define MMAP_PASSED           (1 << 6)

//Is there any information about the drive?
#define DRIVE_INFO_PASSED     (1 << 7)

//Was the config table passed?
#define CONFIG_TABLE_PASSED   (1 << 8)

//Does the bootloader have a name?
#define BOOTLOADER_NAME_PASSED (1 << 9)

//Was the APM table passed?
#define APM_TABLE_PASSED      (1 << 10)

//Was video information passed?
#define VBE_INFO_PASSED       (1 << 11)
#define FRAMEBUFFER_INFO_PASSED (1 << 12)

//Based on sample multiboot.h
//provided in the grub manual

//This is what is passed to kmain
typedef struct multiboot_info{

   //Used to test what we were passed
   uint32_t flags;

   //Upper and lower amounts
   //of memory, in KB
   uint32_t mem_lower;
   uint32_t mem_upper;

   //The partition booted off of
   uint32_t boot_part;

   //Command line args
   uint32_t cmdline;

   //Boot module list
   uint32_t num_mods;
   uint32_t *mods;

   //Either the symbol table for
   //the a.out/ELF file booted from
   union{
      mb_aout_symbol_table_t aout_sym;
      mb_elf_table_t elf_sym;
   } u; 

   //Memory map information
   uint32_t mmap_len;
   uint32_t mmap_addr;

   //Drive information buffer
   uint32_t drive_len;
   uint32_t drive_addr;

   //ROM configuration table
   uint32_t config_table;

   //Boot loader name
   char *bootloader_name;

   //APM table
   uint32_t apm_table;

   //Video information
   uint32_t vbe_control_info;
   uint32_t vbe_mode_info;
   uint16_t vbe_mode;
   uint16_t vbe_interface_seg;
   uint16_t vbe_interface_off;
   uint16_t vbe_interface_len;

   uint64_t fb_addr;
   uint32_t fb_pitch;
   uint32_t fb_width;
   uint32_t fb_height;
   uint8_t  fb_bpp;

#define FB_INDEXED 0
#define FB_RGB     1
#define FB_EGA_TEXT 2
   uint8_t fb_type;

   union{

      struct{
         uint32_t fb_palette_addr;
         uint32_t palette_num_colors;
      };
      struct{
         uint8_t fb_red_field_position;
         uint8_t fb_red_mask_size;
         uint8_t fb_green_field_position;
         uint8_t fb_green_mask_size;
         uint8_t fb_blue_field_position;
         uint8_t fb_blue_mask_size;
      };
   };
}multiboot_info_t;

typedef struct mb_mmap_entry{
   uint32_t size;
   uint64_t addr;
   uint64_t len;

#define MB_MEM_AVAILABLE 1
#define MB_MEM_RESERVED	 2
#define MB_MEM_ACPI_RECLAIM 3
#define MB_MEM_NVS       4
#define MB_MEM_BADRAM    5
   uint32_t type;
}mb_mmap_entry_t;


typedef struct multiboot_mod_list{
   //Module starts at mod_start and ends
   //at mod_end inclusively
   uint32_t mod_start;
   uint32_t mod_end;

   //Command line args for module
   uint32_t cmdline;

   //Padding
   uint32_t padding_not_used_ever;
}multiboot_mod_list_t;

//apm bios info
typedef struct mb_apm_info{
   uint16_t version;
   uint16_t cseg;
   uint32_t offset;
   uint16_t cseg_16;
   uint16_t dseg;
   uint16_t flags;
   uint16_t cseg_len;
   uint16_t cseg_16_len;
   uint16_t dseg_len;
}mb_apm_info_t;
