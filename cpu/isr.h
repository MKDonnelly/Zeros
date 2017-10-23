
#pragma once

#include "types.h"
#include "idt.h"
#include "../drivers/vgatext/print.h"
#include "../lib/string.h"
#include "../drivers/portio.h"

//Defined in assembly.asm
extern void isr0();
extern void isr33();

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
void keyboard_handler();

