#pragma once
#include <lib/types.h>

typedef struct{

   //Saved GP registers
   uint64_t r15, r14, r13, r12, r11, r10, r9, r8, 
            rdi, rsi, rbp, rbx, rdx, rcx, rax;

   //The interrupt number and error code
   uint64_t number, error;   

   //Automatically pushed by CPU
   uint64_t rip, cs, rflags, rsp, ss;
}context_t;

