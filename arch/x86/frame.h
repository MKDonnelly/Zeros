#pragma once

//The frame pool ONLY deals with physical addresses.
//It never dereferences memory (this is basically
//just accounting for frames), so this is no problem.

#include <lib/types.h>

#define ARCH_FRAME_SIZE 0x1000

//Free the frame given its address
void framepool_free(uint32_t addr);

//Allocate a frame at a particular address
int8_t framepool_alloc(uint32_t addr);

//Allocate the first free frame and return
//its address for use.
uint32_t framepool_first_free();

//Get the allocation status of a frame
//1 = allocated, 0 = free
uint8_t framepool_frame_stat(uint32_t);

//Setup global variables defined in frame.c to manage
//pages from paddr_start to paddr_start + length.
void framepool_create(uint32_t paddr_start, uint32_t length);
