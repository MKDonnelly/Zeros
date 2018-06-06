#include <drivers/net/rtl8139.h>

#include <arch/current_arch>
#include <drivers/pci/pci.h>
#include <lib/print.h>
#include <kernel/mm/heap.h>

netdev_t rtl8139_device;
#define MACADDR_LEN 6

//Offsets from base register

//All 1 byte registers
#define MAC0	0
#define MAC1	1
#define MAC2	2
#define MAC3	3
#define MAC4	4
#define MAC5	5

//All 1 byte registers
#define MAR0	8
#define MAR1	9
#define MAR2	10
#define MAR3	11
#define MAR4	12
#define MAR5	13
#define MAR6	14
#define MAR7	15

//4 byte register
#define RBSTART	0x30

//1 byte register
#define CMD	0x37

//2 byte registers
#define IMR	0x3C
#define ISR	0x3E

#define CONFIG_1	0x52

#define RST_BIT	(1 << 4)

//Transmit registers and status registers
//Each 32 bits long
#define TRANS1	0x20
#define TRANS2	0x24
#define TRANS3	0x28
#define TRANS4	0x2C

//transmit status/command regs (32 bits)
#define TSTAT1	0x10
#define TSTAT2	0x14
#define TSTAT3	0x18
#define TSTAT4	0x1C

//Bus and slot refer to the PCI bus and slot this card is on
void rtl8139_init(uint8_t bus, uint8_t slot){

   //Setup the base address for the device as well as reading the MAC
   rtl8139_device.basereg = (pci_get_barnum(bus, slot, 0) & 0xFFFFFFFC);
   rtl8139_device.mac_addr = k_malloc(MACADDR_LEN, 0);
   for(int i = 0; i < MACADDR_LEN; i++){
      rtl8139_device.mac_addr[i] = portb_read(rtl8139_device.basereg + i);
   }


   //Setup PCI bus mastering. Read command register from PCI config space
   //for device, set bit 2 (bus mastering bit), and write back.
   uint16_t cmd_reg = pci_get_cmdreg(bus, slot);
   cmd_reg |= 0x2;      
   pci_write_cmdreg(bus, slot, cmd_reg);

   //Power on the NIC by sending 0x00 to the CONFIG_1 register (0x52) 
   //to set the LWAKE + LWPTN to active high.
   portb_write( rtl8139_device.basereg + CONFIG_1, 0x00);
      
   //Reset the device to get into a known state. Do this by sending
   //0x10 to the CMD register.
   portb_write( rtl8139_device.basereg + CMD, RST_BIT );
   //Wait for the reset to complete by checking the RST bit.
   //once clear, the device is ready.
   for(int i = 1000; i > 0; i--){
      if( (portb_read( rtl8139_device.basereg + CMD) & RST_BIT) == 0) break;
      for(int i = 100; i > 0; i--);
   }
//   while( portb_read(rtl8139_device.basereg + CMD) & RST_BIT );

   //Initilize receive buffer. Allocate 20K and get its phys addr
   char *input_buf = k_malloc( 0x1000 * 5, 0x1000 );
   uint32_t phys_addr = (uint32_t)input_buf - 0xC0000000;
   rtl8139_device.input_buf = input_buf;

   //Tell the device about this address
   portd_write( rtl8139_device.basereg + RBSTART, phys_addr );

   //Set the TOK and ROK bits high
   portw_write( rtl8139_device.basereg + IMR, 0x0005);

   //set WARP bit
   portd_write( rtl8139_device.basereg + 0x44, 0xf | (1 << 7));

   //enable receive and transmit. Set RE and TE bits high
   portb_write( rtl8139_device.basereg + CMD, 0x0C);
}

void rtl8139_test_send(){
   //Test send a packet from the first buffer
   char *packet = k_malloc( 100, 0x1000 );
   for(int i = 0; i < 100; i++)
      packet[i] = i;

   //This goes into TRANS1
   uint32_t phys_addr = (uint32_t)packet - 0xC0000000;
   k_printf("Physical addr: %x\n", phys_addr);
   
   //This goes into TSTAT1
   uint32_t tstat = 100;

   //Write the registers
   portd_write( TRANS1 + rtl8139_device.basereg, phys_addr );
   portd_write( TSTAT1 + rtl8139_device.basereg, tstat );

   //Wait for the packet to complete sending
   while( (portd_read( TSTAT1 + rtl8139_device.basereg ) & 0x00001000) == 0);
   k_printf("Packet Sent!!!!!\n");

}
