#pragma once

//C functions headers for assembly routines in
//descriptors.asm
void tss_load();
void idt_load(void*);
void gdt_load(void*);
