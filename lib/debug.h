#pragma once

#define ASSERT( condition ) if( ! (condition) ) { \
                                k_printf("WARNING: ASSERT FAILED IN %s AT LINE %d\n", __FILE__, __LINE__); \
                            }

