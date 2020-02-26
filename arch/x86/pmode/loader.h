#pragma once
#include <lib/types.h>
#include <lib/elf32.h>
#include <arch/x86/pmode/paging.h>

uint32_t arch_create_from_elf(Elf32_Ehdr *elf_program, pd_t *task_pd, int skip);
