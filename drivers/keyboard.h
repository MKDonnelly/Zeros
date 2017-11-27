#pragma once

#include <isr.h>
#include <types.h>

#define KEYBOARD_BUFFER_SIZE 128
#define KEYBOARD_IRQ 1

#define KEYBOARD_STATUS_P 0x64
#define KEYBOARD_DATA_P 0x60

#define KEYBOARD_CONTROLLER_P 0x64
//Is output buffer full (1) or not (0)
#define KCP_OUT_BUF_S         1<<0
//Is input buffer full (1) or not (0)
#define KCP_IN_BUF_S          1<<1
//System flag. power on reset (0), self-test (1)
#define KCP_SYS_FLAG          1<<2
//Last i/o was data (0) (port 0x60)
//or command (port 0x64)
#define KCP_CMD_DATA          1<<3
//Keyboard is locked (0) or not locked (1)
#define KCP_LOCKED            1<<4
//Auxiliary output buffer.
// Ps/2:
//     keyboard data (0)
//     mouse data (1)
// AT:
//     (0) ok flag
//     (1) timeout when accessing keyboard controller
#define KCP_AUX_BUF           1<<5

#define KEYBOARD_ENCODER_P    0x60
#define KEP_SET_LEDS          0xED
#define KEP_ALT_SCAN_CODES    0xF0
#define KEP_AUTOREPEAT        0xF3


#define SHIFT_RIGHT_KEY 54
#define SHIFT_LEFT_KEY  42 
#define INSERT_KEY      1

extern char keyboard_map[];

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
void add_keyboard_buffer(int8_t);
void getline(int8_t*);

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
extern int8_t keycode_to_char[256];
extern int8_t shift_keycode_to_char[256];
