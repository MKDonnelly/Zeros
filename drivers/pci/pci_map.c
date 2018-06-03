#include <drivers/pci/pci_map.h>

//Vendor id -> vendor name
#define VENDOR_MAXLEN 64
typedef struct{
   uint16_t vendor_id;
   char vendor_name[VENDOR_MAXLEN];
}pci_vmap_t;

pci_vmap_t vendors[] = {
   {0x8086, "Intel"},
   {0x10EC, "Realtek"},
   {0x1022, "AMD"},
   {0x1AF4, "Virtio"},
};
#define TOTAL_VENDORS ( sizeof(vendors) / sizeof(vendors[0]) )

//Device id -> device name
#define DEVICE_MAXLEN 64
typedef struct{
   uint16_t device_id;
   char device_name[DEVICE_MAXLEN];
   //Used when enumerating the PCI bus to
   //initilize the device
   void (*device_init)();
}pci_dmap_t;

extern void rtl8139_init();
pci_dmap_t devices[] = {
   {0x8029, "NE2000 Ethernet", NULL},
   {0x100E, "E1000 Ethernet", NULL},
   {0x2000, "PCNET Ethernet", NULL},
   {0x8139, "rtl8139 Ethernet", &rtl8139_init},
   {0x1000, "Virtio", NULL},
};

#define TOTAL_DEVICES ( sizeof(devices) / sizeof(devices[0]) )

char *pci_lookup_vendor(uint16_t vendor_id){
   for(int i = 0; i < TOTAL_VENDORS; i++){
      if( vendors[i].vendor_id == vendor_id )
         return &vendors[i].vendor_name;
   }
   return NULL;
}

char *pci_lookup_device(uint16_t device_id){
   for(int i = 0; i < TOTAL_DEVICES; i++){
      if( devices[i].device_id == device_id )
         return &devices[i].device_name;
   }
   return NULL;
}

char *pci_lookup_devinit(uint16_t device_id){
   for(int i = 0; i < TOTAL_DEVICES; i++){
      if( devices[i].device_id == device_id )
         return devices[i].device_init;
   }
   return NULL;

}
