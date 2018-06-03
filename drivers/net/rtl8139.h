#pragma once

#include <drivers/net/netdev.h>
#include <lib/types.h>
#include <arch/current_arch>

extern netdev_t rtl8139_device;
void rtl8139_init(uint8_t bus, uint8_t slot);
char *rtl8139_get_mac(netdev_t *dev);
void rtl8139_test_send();
