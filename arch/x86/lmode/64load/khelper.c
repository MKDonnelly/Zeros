#include <lib/elf64.h>
#include <lib/types.h>

//The 32 bit header calls this with the address of the
//64 bit elf kernel image embedded within it. This uses
//the elf header to determine where to start.
uint32_t get_kernel64_addr(uint32_t addr_kernel64){
   Elf64_Ehdr *kernel64_hdr = (Elf64_Ehdr*)addr_kernel64;
   Elf64_Phdr *pheader = (Elf64_Phdr*)(addr_kernel64 + (int)kernel64_hdr->e_phoff);

   int size = sizeof(long long int);
   size++;
   uint32_t start_addr = addr_kernel64 + pheader->p_offset;
   
   return start_addr;
}
