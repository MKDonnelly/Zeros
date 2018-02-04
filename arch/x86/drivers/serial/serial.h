#pragma once

#include <arch/x86/portio.h>

#define COM1_IOPORT 0x3F8
#define COM2_IOPORT 0x2F8
#define COM3_IOPORT 0x3E8
#define COM4_IOPORT 0x2E8

void sp_init();
int sp_transmit_empty();
void sp_putchar(char);
void sp_putstr(char*);
