#pragma once
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

//Used to align a memory address. Take the address
//and add the align size minus 1 to have the address
//rounded up. Then and this with the align size minus
//1. The last part (~(align_size-1)) will set all of
//the bits from the first set to the end. i.e.
// 0x10 = 0b00010000
// (0x10-1) = 0b00001111
// ~(0x10-1) = 0b11110000 <-This is what we want to and with
#define ALIGN_ON( addr, align_size ) \
        ( ((addr) + align_size - 1) & ~(align_size-1))

#define ALIGN_DOWN( addr, align_size ) \
	( ((addr) & ~(align_size-1)) )

//Given an address, returns true if the address is aligned.
#define IS_ALIGNED_ON( addr, align_size ) \
        ( ALIGN_ON( addr, align_size ) == addr ? 1 : 0 )
