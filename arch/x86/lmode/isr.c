#include <arch/x86/lmode/context.h>

void main_interrupt_handler(context_t r){
   k_printf("Got interrupt %d\n", r.number);
   apic_eoi();
}
