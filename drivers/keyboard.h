#pragma once

#include "../cpu/isr.h"
#include "../lib/types.h"

#define KEYBOARD_PIC_MASK 0xFD
#define KEYBOARD_BUFFER_SIZE 128

extern unsigned char keyboard_map[];

//Characters are appended to the end of the array
//When the array is full, characters are shifted
//down and the first is removed.
extern char KEYBOARD_BUFFER[ KEYBOARD_BUFFER_SIZE ];
//Number of characters in the buffer.
extern int KEYBOARD_BUFFER_CHARS;

void init_keyboard();
//Called when interrupt #33 is
//raised
void keyboard_handler();

//Manages KEYBOARD_BUFFER
void add_keyboard_buffer(char);
void getline(char*);
