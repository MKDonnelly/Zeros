#pragma once

#include <lib/types.h>

//Routines to deal with bitfields.
//These can set an arbitrary bit in
//an arbitrary piece of memory.

void bit_set(void *bitfield, size_t bit_idx);
void bit_clear(void *bitfield, size_t bit_idx);
size_t bit_get(void *bitfield, size_t bit_idx);
