#include <drivers/pci/pci.h>

#define CONFIG_VENDOR	0
#define CONFIG_DEVID	2


uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset){
   uint32_t address;
   uint32_t lbus = (uint32_t)bus;
   uint32_t lslot = (uint32_t)slot;
   uint32_t lfunc = (uint32_t)func;
   uint16_t tmp = 0;

   //Create configuration address
   address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | 
             (offset &0xFC) | ((uint32_t)0x80000000));

   //Write out address
   portd_write(PCI_CONFIG_ADDRESS, address);

   //Read in data
   tmp = (uint16_t)((portd_read(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
   return tmp;
}

uint16_t pci_get_vendor(uint8_t bus, uint8_t slot){
   return pci_config_read_word(bus, slot, 0, CONFIG_VENDOR);
}

uint16_t pci_get_devid(uint8_t bus, uint8_t slot){
   return pci_config_read_word(bus, slot, 0, CONFIG_DEVID);
}

uint8_t pci_get_class(uint8_t bus, uint8_t slot){
   return (pci_config_read_word(bus, slot, 0, 0xA) >> 8) & 0xFF;
}

uint8_t pci_get_subclass(uint8_t bus, uint8_t slot){
   return pci_config_read_word(bus, slot, 0, 0xA) & 0xFF;
}

uint32_t pci_get_barnum(uint8_t bus, uint8_t slot, uint8_t num){
   uint8_t off = 0x10 + num * 4;
   
   uint16_t low_half = pci_config_read_word(bus, slot, 0, off);
   uint16_t high_half = pci_config_read_word(bus, slot, 0, off+2);

   return low_half & (high_half << 16);
}

uint8_t pci_get_htype(uint8_t bus, uint8_t slot){
   return pci_config_read_word(bus, slot, 0, 0xE) & 0xFF;
//   return (uint8_t)pci_config_read_word(bus, slot, 0, 0x0E);
}
