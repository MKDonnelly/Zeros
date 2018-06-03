#pragma once

#include <lib/types.h>

typedef struct{
   uint32_t basereg;
   char *mac_addr;
   char *input_buf;
}netdev_t;

