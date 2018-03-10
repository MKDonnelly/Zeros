#pragma once

//The master file for the x86 architecture.
//This is all anything outside of arch/x86
//needs to include; anything else is strictly
//off limits

#include <arch/x86/paging.h>
#include <arch/x86/isr.h>
#include <arch/x86/cpu.h>
#include <arch/x86/context.h>

#include <arch/x86/drivers/keyboard.h>
#include <arch/x86/drivers/mouse.h>
#include <arch/x86/drivers/timer.h>
#include <arch/x86/drivers/serial/serial.h>
#include <arch/x86/drivers/cmos.h>

#include <arch/x86/drivers/vgacommon/modeset.h>
#include <arch/x86/drivers/vgacommon/vgacommon.h>
#include <arch/x86/drivers/vgacommon/vgafont.h>

extern void arch_jump_to_thread(thread_context_t*);

void arch_init_system();
