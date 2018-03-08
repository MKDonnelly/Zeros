#include <arch/x86/vm.h>

uint32_t virt_to_phys(uint32_t addr, page_directory_t *dir){
   //To find the physical address, we perform two steps
   // (1) Locate the page that the address is on and get
   //     the frame address.
   // (2) From addr, strip off the page specifier bits and 
   //     get the offset of the address within the virtual address.
   //     we then assume that the offset within the frame is the same.
   uint32_t frame_addr = get_frame( get_page( addr, 0, dir ) );

   //This bit operation turns the lower 0's into 1's and zeros
   //out everything else. 0x1000 => 0x0FFF
   uint32_t offset_in_frame = addr & ~( -ARCH_FRAME_SIZE );

   return (frame_addr | offset_in_frame);
}

//Copies a buffer to a physical address in memory
//Used when loading in userland processes
void copy_to_physical(char *buf, int amount, uint32_t addr){

   //Get the physical address of the buffer. This is needed since
   //we deal with physical, not virtual, addresses when copying.
   uint32_t buf_physical = virt_to_phys( (uint32_t)buf, kernel_page_dir );

   //Disable paging and interrupts
   arch_disable_ints();
   arch_disable_paging();

   //Copy over buffer
   memcpy( (char*)addr, (char*)buf_physical, amount );

   //Re-enable paging
   arch_enable_paging();
   arch_enable_ints();
}


