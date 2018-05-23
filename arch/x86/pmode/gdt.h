#pragma once

#include <lib/types.h>

//Null, kernel data/code, userland
//data/code, TSS
#define GDT_ENTRIES 6

//Masks for flags parameter
#define GDT_RLEVEL_0    (0 << 0)
#define GDT_RLEVEL_3    (0b11 << 0)

#define GDT_ACCESSED    (1 << 3)
#define GDT_NOT_ACCESSED (0 << 3)

#define GDT_RW          (1 << 4)
#define GDT_RO          (0 << 4)

#define GDT_NOCONFORM   (1 << 5)
#define GDT_CONFORM     (0 << 5)

#define GDT_CODET       (1 << 6)
#define GDT_DATAT       (0 << 6)

#define GDT_STYPE       (0 << 7)
#define GDT_CDTYPE      (1 << 7)

#define GDT_GRAN4K      (1 << 8)
#define GDT_GRAN1B      (0 << 8)

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
  bit 3: no purpose specified, OS may use as pleases
*/
   int8_t available : 1; //NOT USED
   int8_t long_mode_code : 1; //Always 0 in protected mode.
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
void gdt_set_entry( gdt_entry_t *gdt_entry, uint32_t limit, uint32_t base,
                    uint32_t flags);
