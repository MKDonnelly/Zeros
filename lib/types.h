#pragma once

#define NULL ((void*)0)
#define CHAR_BITS ( sizeof(char) * 8 )

typedef char               int8_t;
typedef unsigned char      uint8_t;
typedef short int          int16_t;
typedef unsigned short int uint16_t;
typedef int                int32_t;
typedef unsigned int       uint32_t;

//va_args type and macro
typedef char* va_arg;
#define va_start( arg ) ( (char*)&arg + sizeof(arg) )
#define va_get( args, type ) ( *((type*)args) ) ; (args += sizeof(type))
