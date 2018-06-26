#pragma once

#include <lib/types.h>

//PCI_CONFIG_ADDRESS format 
//Bit 31 - Enable bit
//Bits 30-24 - Reserved
//Bits 23-16 - Bus Number
//Bits 15-11 - Device Number
//Bits 10-8 - Function Number
//Bits 7-2 - Register number
//Bits 1-0 - Always 00
#define PCI_CONFIG_ADDRESS	0xCF8
#define PCI_CONFIG_DATA		0xCFC

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, 
                              uint8_t func, uint8_t offset);
void pci_config_write_word(uint8_t bus, uint8_t slot, uint8_t func, 
                           uint8_t offset, uint16_t new_val);

uint16_t pci_get_vendor(uint8_t bus, uint8_t slot);
uint16_t pci_get_devid(uint8_t bus, uint8_t slot);
uint8_t pci_get_class(uint8_t bus, uint8_t slot);
uint8_t pci_get_subclass(uint8_t bus, uint8_t slot);
uint32_t pci_get_barnum(uint8_t bus, uint8_t slot, uint8_t num);
uint16_t pci_get_cmdreg(uint8_t bus, uint8_t slot);
uint8_t pci_get_intnum(uint8_t bus, uint8_t slot);
void pci_write_cmdreg(uint8_t bus, uint8_t slot, uint16_t new_val);
uint8_t pci_get_htype(uint8_t bus, uint8_t slot);

void pci_enumerate();
