#pragma once

#include <lib/types.h>

//Routines to deal with bitfields.
//These can set an arbitrary bit in
//an arbitrary piece of memory.

void bitSet(void*,int);
void bitClear(void*,int);
uint8_t bitGet(void*,int);

