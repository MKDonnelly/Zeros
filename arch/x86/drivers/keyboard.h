#pragma once

#include <lib/types.h>

#define KEYBOARD_IRQ 1
#define KEYBOARD_INTERRUPT 33

//              Keyboard controller constants
#define KEYBOARD_CONTROLLER_P 0x64

//Is output buffer full (1) or not (0)
#define KC_OUTPUTBUF_M          1<<0
//Is input buffer full (1) or not (0)
#define KC_INPUTBUF_M           1<<1
//System flag. power on reset (0), self-test (1)
#define KC_FLAG_M               1<<2
//Last i/o was data (0) (port 0x60)
//or command (port 0x64)
#define KC_LAST_M               1<<3
//Keyboard is locked (0) or not locked (1)
#define KC_LOCKED_M             1<<4
//Auxiliary output buffer.
// Ps/2:
//     keyboard data (0)
//     mouse data (1)
// AT:
//     (0) ok flag
//     (1) timeout when accessing keyboard controller
#define KC_AUXBUF_M             1<<5
//End of keyboard controller constants


//             Keyboard encoder constants
#define KEYBOARD_ENCODER_P    0x60

//Keyboard encoder commands
#define KE_SET_LEDS_C         0xED
//Masks for the above
#define KE_SCROLL_M           0x1
#define KE_NUMLOCK_M          0x2
#define KE_CAPLOCK_M          0x4
//
#define KE_ECHO_C             0xEE
#define KE_ALT_SCAN_CODES_C   0xF0
#define KE_KBD_ID_C           0xF2
#define KE_AUTOREPEAT_C       0xF3
#define KE_ENABLE_KBD_C       0xF4
#define KE_RESET_WAIT_C       0xF5
#define KE_RESET_SCAN_C       0xF6
#define KE_ALL_AUTOREP_C      0xF7
#define KE_ALL_MAKEBREAK_C    0xF8
#define KE_ONLY_MAKE_C        0xF9
#define KE_AUTOREP_MAKEBREAK_C 0xFA
#define KE_KEY_AUTOREP_C      0xFB
#define KE_KEY_MAKEBREAK_C    0xFC
#define KE_KEY_BREAK_C        0xFD
#define KE_RESEND_C           0xFE
#define KE_RESET_C            0xFF

#define INSERT_KEY      1
#define SHIFT_LEFT_KEY  42 
#define SHIFT_RIGHT_KEY 54

void arch_keyboard_init( void (*keypress_callback)(char) );
void arch_keyboard_register_callback(void (*keypress_callback)(char) );

//Various routines that communicate
//to the keyboard controller/encoder
//Takes KE_{SCROLL,NUMLOCK,CAPLOCK}_M
void kb_set_leds(char flags);
void kbd_ctrl_send_command(uint8_t);
void kbd_enc_send_command(uint8_t);

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
