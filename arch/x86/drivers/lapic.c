#include <arch/x86/drivers/lapic.h>
#include <lib/types.h>
#include <arch/x86/lmode/paging.h>
#include <arch/x86/portio.h>

static void disable_pic(){
   portb_write(0x20, 0x11);
   portb_write(0xa0, 0x11);

   portb_write(0x21, 0xe0);
   portb_write(0xa1, 0xe8);

   portb_write(0x21, 4);
   portb_write(0xa1, 2);

   portb_write(0x21, 1);
   portb_write(0xa1, 1);

   portb_write(0x21, 0xff);
   portb_write(0xa1, 0xff);
}

void apic_eoi(){
   uint32_t *apic_eoi = (uint32_t*)APIC_EOI;
   *apic_eoi = 0;
}

static inline void apic_set_init_count(uint32_t count){
   *((uint32_t*)APIC_TIMER_ICOUNT) = count;
}

static inline void apic_set_div(uint32_t div){
   *((uint32_t*)APIC_TIMER_DIV) = div;
}

static inline void apic_timer_cfg(uint32_t conf){
   *((uint32_t*)APIC_TIMER) = conf;
}

static inline void apic_spur_enable(){
   *((uint32_t*)APIC_SPURIOUS) |= ASPUR_ENABLE;
}

void apic_enable(){
   disable_pic();

   //Make sure to identity map in the APIC registers
   vm_pmap(APIC_BASE, APIC_BASE, kernel_pdir, VMAP_W | VMAP_USR);

   //Set APIC enable bit in Spurrios interrupt register
   apic_spur_enable();
   apic_set_init_count(0xfffff);
   apic_set_div(DIVIDE_128);
   apic_timer_cfg(TIMER_PERIODIC | APIC_INT);
}
