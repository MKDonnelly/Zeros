#pragma once

#include <arch/x86/timing.h>
#include <kernel/sched.h>

//Use the system clock to implement a delay
void k_delays(int);
void k_delayms(int);

