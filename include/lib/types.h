#pragma once

#define NULL ((void*)0)
#define CHAR_BITS ( sizeof(char) * 8 )

typedef char               bool;
typedef char               int8_t;
typedef unsigned char      uint8_t;
typedef short int          int16_t;
typedef unsigned short int uint16_t;
typedef int                int32_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
typedef long long int      int64_t;
typedef unsigned long long int uint64_t;

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

//Given an address, returns true if the address is aligned.
#define IS_ALIGNED_ON( addr, align_size ) \
        ( ALIGN_ON( addr, align_size ) == addr ? 1 : 0 )

//va_args type and macro
typedef char* va_arg;
#define va_start( arg ) ( (char*)&arg + sizeof(arg) )
#define va_get( args, type ) ( *((type*)args) ) ; (args += sizeof(type))
