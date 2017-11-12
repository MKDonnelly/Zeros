
#include "va_list.h"

// Define the width of the stack 
// as the width of an int (since
// an int is 32 bits)
#define STACK_SIZE int

#define VA_SIZE(TYPE)                          \
	((sizeof(TYPE) + sizeof(STACK_SIZE-1)  \
	  & ~(sizeof(STACK_SIZE)-1))
#define va_start(AP, LASTARG)  \
        (AP=((va_list)&(LASTARG) + VA_SIZE(LASTARG))

#define va_end(AP)

#define va_arg(AP, TYPE)  \
        (AP += VA_SIZE(TYPE),*((TYPE*)(AP - VA_SIZE(TYPE))))
