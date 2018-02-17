#pragma once

#include <lib/types.h>
#include <arch/x86/portio.h>
#include <arch/x86/drivers/vgacommon/vgacommon.h>
#include <arch/x86/isr.h>

#define MOUSE_ISR 12
#define MOUSE_INTERRUPT MOUSE_ISR + 32

#define MOUSE_DATA_P 0x60
#define MOUSE_CMD_P  0x64

void mouse_init();
void mouse_interrupt_handler();
