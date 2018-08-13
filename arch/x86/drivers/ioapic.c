#include <arch/x86/drivers/ioapic.h>
#include <lib/types.h>

void ioapic_set_irq(char irq, uint64_t apic_id, char vector){
/*   uint32_t low_index = 0x10 + irq*2;
   uint32_t high_index = 0x10 + irq*2 + 1;
   
   uint32_t high = ioapic_read(high_index);
   //set APIC ID
   high &= ~0xff000000;
   high |= apic_id << 24;
   ioapic_write(high_index, high);

   uint32_t low = ioapic_read(low_index);

   //unmask IRQ
   low &= ~(1<<16);

   //set physical delivery mode
   low &= ~(1<<11);

  //set to fixed delivery mode
  low &= ~0x700;

   //set delivery vector
   low &= ~0xff;
   low |= vector;

   ioapic_write(low_index, low);*/
}
