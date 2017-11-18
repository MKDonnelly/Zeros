#pragma once

#include "../cpu/isr.h"
#include "../lib/types.h"

#define KEYBOARD_BUFFER_SIZE 128
#define KEYBOARD_IRQ 1

#define KEYBOARD_STATUS_P 0x64
#define KEYBOARD_DATA_P 0x60

#define SHIFT_RIGHT_KEY 54
#define SHIFT_LEFT_KEY  42 

extern unsigned char keyboard_map[];

//Characters are appended to the end of the array
//When the array is full, characters are shifted
//down and the first is removed.
extern char KEYBOARD_BUFFER[ KEYBOARD_BUFFER_SIZE ];
//Number of characters in the buffer.
extern int KEYBOARD_BUFFER_CHARS;

void init_keyboard();
//Manages keyboard input
void keyboard_handler();

//Manages KEYBOARD_BUFFER
void add_keyboard_buffer(char);
void getline(char*);


// '\0' = Not implemented
// Ascii DC1 = Arrow up
// Ascii DC2 = Arrow Down
// Ascii DC3 = Arrow left
// Ascii DC4 = Arrow Right
// Insert = Start of Heading
// Home = Start Of Text
// Page Up = End Of Text
// End = End Of Transmission
// Page Down = Enquiry
// CTRL = Acknowledge
// Alt = Shift Out
extern char keycode_to_char[256];
extern char shift_keycode_to_char[256];
