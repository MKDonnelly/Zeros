#include <arch/x86/drivers/lapic.h>
#include <lib/types.h>
#include <arch/x86/lmode/paging.h>

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

void apic_enable(){
   asm("cli");
   disable_pic();

   //Make sure to identity map in the APIC registers
   vm_pmap(APIC_BASE, APIC_BASE, kernel_pdir, VMAP_W | VMAP_USR);

   //Set APIC enable bit in Spurrios interrupt register
   *((uint32_t*)APIC_SPURIOUS) |= (1<<8);
 
   uint32_t *timer_reg = (uint32_t*)APIC_TIMER;
   uint32_t *timer_init_cnt = (uint32_t*)APIC_TIMER_COUNT;
   *timer_init_cnt = 0x100;
  
   uint32_t *divide = (uint32_t*)(APIC_BASE + 0x3E0);
   *divide = 0xA;

   uint32_t *timer = (uint32_t*)(APIC_BASE + 0x320);
   *timer = (1<<17) | 32;

   k_printf("%x\n", *((uint32_t*)APIC_VERSION));

//   asm("sti");
}
