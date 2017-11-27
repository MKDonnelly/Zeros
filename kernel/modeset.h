#pragma once
#include <portio.h>

//Much of this taken from 
//http://files.osdev.org/mirrors/geezer/osd/graphics/modes.c

//                          VGA 13h mode
//=========================================================
#define VGA13H_MEM 0xA0000
#define VGA13H_MEM_END   0xA0000 + 320 * 200

#define   VGA13H_BLACK    0,
#define   VGA13H_BLUE     1
#define   VGA13H_GREEN    2
#define   VGA13H_CYAN     3
#define   VGA13H_RED      4
#define   VGA13H_MAGENTA  5
#define   VGA13H_BROWN    6
#define   VGA13H_LGREY    7
#define   VGA13H_DGREY    8
#define   VGA13H_LBLUE    9
#define   VGA13H_LGREEN   10
#define   VGA13H_LCYAN    11
#define   VGA13H_LRED     12
#define   VGA13H_LMAGENTA 13
#define   VGA13H_YELLOW   14
#define   VGA13H_WHITE    15

//This array defines the register state
//for vga mode 13h
extern unsigned char vga_13h_regs[];

//This function takes an array of registers
//and implements the given mode.
void write_regs(unsigned char*);

