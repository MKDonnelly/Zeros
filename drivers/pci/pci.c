#include <drivers/pci/pci.h>

#include <lib/print.h>

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

void pci_config_write_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t new_val){
   uint32_t address;
   uint32_t lbus = (uint32_t)bus;
   uint32_t lslot = (uint32_t)slot;
   uint32_t lfunc = (uint32_t)func;
   uint16_t tmp = 0;

   //Create configuration address
   address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | 
             (offset & 0xFC) | ((uint32_t)0x80000000));

   //Write out address
   portd_write(PCI_CONFIG_ADDRESS, address);

   //Write out data
   portd_write(PCI_CONFIG_DATA, new_val );
}


uint32_t pci_config_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset){
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
   //Return data
   return portd_read(PCI_CONFIG_DATA);
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

uint16_t pci_get_cmdreg(uint8_t bus, uint8_t slot){
   return pci_config_read_word(bus, slot, 0, 0x6 );
}

void pci_write_cmdreg(uint8_t bus, uint8_t slot, uint16_t new_val){
   pci_config_write_word(bus, slot, 0, 0x6, new_val);
}

uint32_t pci_get_barnum(uint8_t bus, uint8_t slot, uint8_t num){
   uint8_t off = 0x10 + num * 4;
   
   return pci_config_read_dword(bus, slot, 0, off);
}

uint8_t pci_get_intnum(uint8_t bus, uint8_t slot){
   uint32_t tmp = pci_config_read_dword(bus, slot, 0, 0x3C);
   return (tmp & 0x0000000F);
}

uint8_t pci_get_htype(uint8_t bus, uint8_t slot){
   return pci_config_read_word(bus, slot, 0, 0xE) & 0xFF;
}

void pci_enumerate(){
   for(int bus = 0; bus < 16; bus++){
      for(int slot = 0; slot < 32; slot++){
         uint16_t vendor = pci_get_vendor(bus, slot);
         if( vendor != 0xFFFF ){
            k_printf("PCI Device: ");
            k_printf("Vendor: %x, ", vendor);
            k_printf("DevId: %x, ", pci_get_devid(bus, slot));
            k_printf("Class: %x, ", pci_get_class(bus, slot));
            k_printf("\n     ");
            k_printf("Subclass: %x, ", pci_get_subclass(bus, slot));
            k_printf("HType: %x, \n", pci_get_htype(bus, slot));
            char *dev_name = pci_lookup_device(pci_get_devid(bus, slot));
            void (*dev_init)() = pci_lookup_devinit(pci_get_devid(bus, slot));
            if( dev_name != NULL ) 
               k_printf("Device Name: %s\n", dev_name);
            if( dev_init != NULL )
               dev_init(bus, slot);
            
         }
      }
   }
}
