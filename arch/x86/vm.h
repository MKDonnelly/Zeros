#pragma once

#include <arch/x86/paging.h>
#include <lib/types.h>

extern void arch_disable_paging();
extern void arch_enable_paging();

//uint32_t virt_to_phys(uint32_t,page_directory_t*);
void copy_to_physical(char*,int,uint32_t);

