#pragma once

#include "../../drivers/portio.h"
#include "common.h"

int get_cursor();
void move_cursor(int, int);
void move_cursorl(int);
int k_xy_to_linear(int,int);
