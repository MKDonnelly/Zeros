#pragma once

//Comment out to disable debugging mode
#define DEBUG_ON 1

#define ASSERT( condition ) \
   if( ! (condition) && DEBUG_ON ) { \
       k_printf("WARNING: ASSERT \"%s\" FAILED AT %d %d in function %d\n", #condition, __FILE__, __LINE__, __FUNCTION__); \
   }


   

