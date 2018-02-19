#pragma once

//TODO make these macros or move them to a pure assembly file
#define arch_trigger_interrupt(num) asm volatile("int %0" :: "i"(num):"memory");
#define arch_halt_cpu() asm volatile("hlt")

void arch_stop_cpu();
void arch_disable_ints();
void arch_enable_ints();
