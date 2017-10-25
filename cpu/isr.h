
#pragma once

#include "idt.h"
#include "../drivers/vgatext/print.h"
#include "../drivers/portio.h"

#include "../lib/string.h"
#include "../lib/types.h"

extern unsigned char keyboard_map[128];

//Defined in assembly.asm
extern void isr0();

extern void isr30();
extern void isr31();
extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();


typedef struct {
   udword ds;
   udword edi, esi, ebp, esp, ebx, edx, ecx, eax;
   udword int_no, err_code;
   udword eip, cs ,eflags, useresp, ss;
}reg_struct;

//This function handles the installation
//of each interrupt handler
void install_isrs();

//These are the individual handlers
void zero_divide();
void default_pic();
void keyboard_handler();

