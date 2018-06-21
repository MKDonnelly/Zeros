#pragma once
#include <lib/types.h>
#define TOTAL_INTERRUPTS	256

//16 bytes each
typedef struct{
   uint32_t offset_low : 16;
   uint32_t seg_sel : 16;
   
   uint8_t ist;
   uint8_t flags;
   uint32_t offset_mid : 16;
  
   uint32_t offset_high;

   uint32_t reserved;
}__attribute__((packed)) idt_entry_t;


typedef struct{
   uint16_t limit;
   uint64_t base_addr;   
}__attribute__((packed)) idt_descriptor_t;
