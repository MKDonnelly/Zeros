#pragma once

#define KASSERT( condition )\
     if( ! (condition) ){                               \
        k_printf("ASSERT \"%s\" FAILED\n", #condition); \
     }
