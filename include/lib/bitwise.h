#pragma once

#include <lib/types.h>

//Routines to deal with bitfields.
//These can set an arbitrary bit in
//an arbitrary piece of memory.

void bit_set(void*,int);
void bit_clear(void*,int);
uint8_t bit_get(void*,int);

