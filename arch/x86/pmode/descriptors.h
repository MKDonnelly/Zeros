#pragma once

//C functions headers for assembly routines in
//descriptors.asm
void load_tss();
void arch_load_idt(void*);
void load_gdt(void*);
