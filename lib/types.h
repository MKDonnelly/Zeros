
#pragma once

#define lower_16(address) (u16)( (address) & 0xFFFF )
#define upper_16(address) (u16)( (address >> 16) & 0xFFFF )
#define NULL ((void*)0)

//The size of each data type. Does not seem
//very useful at first, but these can come
//in handy
#define CHAR_SIZE (sizeof(char) * 8)
#define UINT_SIZE (sizeof(unsigned int) * 8)

typedef          char  s8;
typedef unsigned char  u8;
typedef        short   s16;
typedef unsigned short u16;
typedef        int     s32;
typedef unsigned int   u32;

//Make the size of the type more obvious
typedef char sbyte;
typedef unsigned char ubyte;
typedef short sword;
typedef unsigned short uword;
typedef int sdword;
typedef unsigned int udword;

