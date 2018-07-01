#include <arch/x86/lmode/idt.h>

static idt_entry_t idt_table[TOTAL_INTERRUPTS];
static idt_descriptor_t idt_descriptor;

void idt_add_entry(uint64_t int_number, uint64_t handle_function){
   idt_table[int_number].offset_low = handle_function & 0xFFFF;
   idt_table[int_number].offset_mid = (handle_function >> 16) & 0xFFFF;  
   idt_table[int_number].offset_high = (handle_function>>32) & 0xFFFFFFFF;

   idt_table[int_number].ist = 0;
   idt_table[int_number].flags = (1 << 7) | //present
                                 (0b00 << 5) | //dpl
                                 (0xE); //type
                                 
   idt_table[int_number].seg_sel = 0x08; //GDT kernel code
}

void idt_init(){
   memset(idt_table, TOTAL_INTERRUPTS * sizeof(idt_entry_t), 0);
   idt_setup_isrs();

   idt_descriptor.base_addr = (uint64_t)&idt_table;
   idt_descriptor.limit = TOTAL_INTERRUPTS * sizeof(idt_entry_t) - 1;

   arch_disable_ints();
   idt_load(&idt_descriptor);
}
