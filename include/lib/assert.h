#pragma once

#include <lib/print.h>
//Comment out to disable debugging mode
#define DEBUG_ON 1

#define KASSERT( condition ) \
   if( ! (condition) && DEBUG_ON ) { \
       k_printf("WARNING: ASSERT \"%s\" FAILED IN %s LINE %d in function %s\n", #condition, __FILE__, __LINE__, __FUNCTION__); \
   }
