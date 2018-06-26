#pragma once

#include <lib/types.h>

//Routines to deal with bitfields.
//These can set an arbitrary bit in
//an arbitrary piece of memory.
void bit_set(void *bitfield, int index);
void bit_clear(void *bitfield, int index);
uint8_t bit_get(void *bitfield, int index);
