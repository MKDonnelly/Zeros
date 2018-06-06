#pragma once

#include <lib/types.h>

char *pci_lookup_vendor(uint16_t vendor_id);
char *pci_lookup_device(uint16_t devide_id);
void *pci_lookup_devinit(uint16_t device_id);
