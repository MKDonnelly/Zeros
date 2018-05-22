#pragma once

#include <lib/types.h>

void set_kernel_stack(uint32_t);

//Null, kernel data/code, userland
//data/code, TSS
#define GDT_ENTRIES 6

struct gdt_entry{
   int16_t limit_low;    //Limit bits 0-15
   int16_t base_low;       //Base bits 0-15
   int8_t  base_mid;     //Base bits 16-23

/*      flags bits description
  bit 0: present flag. Is this segment present in memory?
         1 for yes, 0 for not
  bit 1-2: Ring level for this descriptor. 00 for ring 0,
           and 11 for ing 3
  bit 3: descriptor type. system segment (0) or a code/data
         segment (1)
  bit 4: is this a code segment (1) or a data segment (0)
  bit 5: conforming bit. 1 if code from a lower ring level
         can call code in this segment, 0 else
  bit 6: is this segment rw (1) or r-only (0)
  bit 7: has this segment been accessed? (1 if yes, 0 else)
*/
  int8_t accessed : 1;
  int8_t rw : 1;
  int8_t conforming : 1;
  int8_t code_or_data : 1;
  int8_t descriptor_type : 1;
  int8_t ring_level : 2;
  int8_t present : 1;


/*     2nd group of flags description
  bit 0: granularity. are limit/base int terms of 4K (1) or 1b (0)
  bit 1: operand size. 0 for 16 bit, 1 for 32 bit
  bit 2: 1 for 64 bit mode code segment, 0 for no
  bit 3: 1 for available for system use, 0 else
*/
   int8_t available : 1;
   int8_t long_mode_code : 1;
   int8_t operand_size : 1;
   int8_t granularity : 1;
   int8_t lim_high : 4;

   int8_t  base_high;     //Base bits 24-31
}__attribute__((packed));
typedef struct gdt_entry gdt_entry_t;


struct gdt_descriptor{
   uint16_t length;
   uint32_t *address;
} __attribute__((packed));
typedef struct gdt_descriptor gdt_descriptor_t;

extern const int16_t gdt_kernel_code;
extern const int16_t gdt_kernel_data;
extern const int16_t gdt_userland_code;
extern const int16_t gdt_userland_data;
extern const int16_t gdt_userland_code_rpl;
extern const int16_t gdt_userland_data_rpl;

//All of the GDT entries will be setup in gdt_init.
//To simplify the process, no support is added to create
//another gdt entry after setup.
void gdt_init();
