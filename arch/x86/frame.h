#pragma once

#include <lib/types.h>
#include <lib/bitwise.h>
#include <lib/memory.h>
#include <kernel/mm/heap.h>

#define ARCH_FRAME_SIZE 0x1000

//Allocate the first free frame and return
//its address for use.
uint32_t first_free_frame();

//Free the frame given its address
void free_frame(uint32_t addr);

//Get the allocation status of a frame
int get_frame_stat(uint32_t);

//Allocate a frame at a particular address
int allocate_frame(uint32_t);

//Initilize the frame global variables.
//This handles calculating memory requirements
//for the bitfield.
void init_frames(uint32_t,uint32_t);
