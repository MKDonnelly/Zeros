#pragma once
#include "../lib/types.h"

extern unsigned int kernel_start_heap;
void *kmalloc(int,int,unsigned int*);

