#pragma once

struct tss{
   uint32_t previous_tss;
   uint32_t esp_ring0, ss_ring0;
   uint32_t esp1_unused;
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
   uint32_t es, cs, ss, ds, fs, gs;
   uint32_t ldt;
   uint16_t trap;
   uint16_t iomap_base;
}__attribute__((packed));
typedef struct tss tss_t;

//TSS used for system
tss_t tss_entry;

//Assembly function to load the tss into the tr
//register after it is in the gdt. In asm_functions.asm
extern void load_tss(void);

