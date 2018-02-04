#pragma once

#define KEYBOARD_BUFFER_MAX 128

void keyboard_main_handler(char key_entered);
void keyboard_register_key_callback(void (*key_callback)(char), char key);
void keyboard_clear_buffer();
char keyboard_getc();
void keyboard_getline(char *buf, int maxlen);


