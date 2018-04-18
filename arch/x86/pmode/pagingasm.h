#pragma once

//Header for use by C programs when using
//pageasm.asm

void arch_disable_paging();
void arch_enable_paging();
void load_pd(uint32_t *page_directory);
void inval_page(uint32_t addr);
