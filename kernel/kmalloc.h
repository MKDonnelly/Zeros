
#pragma once

#include "../lib/types.h"

//udword start_free_mem = 0x1000;
//udword kmalloc(udword size, int align, udword *phys_addr);
void *kmalloc(int,int,unsigned short*);
