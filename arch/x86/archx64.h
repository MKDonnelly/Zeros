#pragma once

#include <arch/x86/drivers/vgacommon/vgacommon.h>
#include <arch/x86/drivers/vga3h/vga3h.h>
#include <arch/x86/lmode/paging.h>
#include <arch/x86/cpu.h>
#include <arch/x86/lmode/cpuid.h>
#include <arch/x86/lmode/spinlock.h>

#include <arch/x86/cpu.h>

void arch_init_system();
