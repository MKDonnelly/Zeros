#pragma once

#include "../cpu/isr.h"
#include "../lib/types.h"

void init_keyboard();
//Called when interrupt #33 is
//raised
void keyboard_handler();
