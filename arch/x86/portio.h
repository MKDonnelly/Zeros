#pragma once

#include <lib/types.h>

int8_t portb_read(uint16_t port);
void portb_write(uint16_t port, uint16_t data);

uint16_t portw_read(uint16_t port);
void portw_write(uint16_t port, uint16_t data);

uint32_t portd_read(uint16_t port);
void portd_write(uint16_t port, uint32_t data);
