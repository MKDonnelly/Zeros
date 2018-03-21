#pragma once

#include <lib/types.h>
#include <arch/x86/portio.h>
#include <arch/x86/drivers/vgacommon/vgacommon.h>
#include <arch/x86/pmode/isr.h>

#define MOUSE_ISR 12
#define MOUSE_INTERRUPT 44

#define MOUSE_DATA_P 0x60
#define MOUSE_CMD_P  0x64

#define MOUSE_AUX_MOUSE 0xA8
#define MOUSE_GET_CONPAQ_C 0x20
#define MOUSE_SEND_COMPAQ_C 0x60
#define MOUSE_WRITE_COMING_C 0xD4
#define MOUSE_SET_DEFAULTS_C 0xF6
#define MOUSE_ENABLE_PSTREAM 0xF4

#define MOUSE_TIMEOUT 10000
#define MOUSE_WRITE_OK_M 0x2
#define MOUSE_READ_OK_M  0x1


//Format of mouse packets
// 1st byte: flags (see below)
// 2nd byte: x movement (negative for left)
// 3rd byte: y movement (negative for down)
// 4th byte (for mouse wheel): z movement (0 no movement,
//         (1 up one click, 0xF down one click) 

//1st byte format: Y overflow, X overflow, Y sign bit, 
//  X sign bit, always 1, middle button, right button, left button
#define MOUSE_MIDDLE_BUTTON 0x4
#define MOUSE_RIGHT_BUTTON 0x2
#define MOUSE_LEFT_BUTTON 0x1

void mouse_init();

//Used by higher level code
typedef struct mouse_packet{
   int8_t delta_x;
   int8_t delta_y;
   int8_t left_button   : 1;
   int8_t middle_button : 1;
   int8_t right_button  : 1;
} mouse_packet_t;

void register_mouse_handler();
void unregister_mouse_handler();
