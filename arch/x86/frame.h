#pragma once

//The frame pool ONLY deals with physical addresses.
//It never dereferences memory (this is basically
//just accounting for frames), so this is no problem.

#include <lib/types.h>

#define ARCH_FRAME_SIZE 0x1000

//Free the frame given its address
void free_frame(uint32_t addr);

//Allocate a frame at a particular address
int8_t allocate_frame(uint32_t addr);

//Allocate the first free frame and return
//its address for use.
uint32_t first_free_frame();

//Get the allocation status of a frame
//1 = allocated, 0 = free
uint8_t get_frame_stat(uint32_t);

//Setup global variables defined in frame.c to manage
//pages from paddr_start to paddr_start + length.
void make_frame_pool(uint32_t paddr_start, uint32_t length);
