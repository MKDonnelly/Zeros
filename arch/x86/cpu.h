#pragma once

//Unfortunatelly, int can only take an immediate, which means that 
//it has to be a macro. That "i" means this must be an immediate
//when passed to the assembly.
#define arch_trigger_interrupt(num) asm volatile("int %0" :: "i"(num));

//Defined in cpu.asm
void arch_stop_cpu();
void arch_halt_cpu();
void arch_enable_ints();
void arch_disable_ints();

