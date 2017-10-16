
#pragma once

typedef          char  s8;
typedef unsigned char  u8;
typedef        short   s16;
typedef unsigned short u16;
typedef        int     s32;
typedef unsigned int   u32;

#define lower_16(address) (u16)( (address) & 0xFFFF )
#define upper_16(address) (u16)( (address >> 16) & 0xFFFF )
