#include <arch/x86/archx64.h>

#include <lib/print.h>
#include <lib/string.h>
#include <kernel/multiboot.h>

void feature_detect(){
   //First get processor identification string
   cpuid_t cpu;
   cpuid(0, &cpu);

   char ident_str[13];
   memcpy( &ident_str[0], &cpu.ebx, 4);
   memcpy( &ident_str[4], &cpu.edx, 4);
   memcpy( &ident_str[8], &cpu.ecx, 4);
   ident_str[12] = 0;

   k_puts("CPU Type: ");
   k_puts(ident_str);
   k_puts("\n");

   //Identify features
   cpuid(1, &cpu);

   if(cpu.edx & CPUID_EDX_FPU) k_puts("FPU supported\n");
   if(cpu.edx & CPUID_EDX_VME) k_puts("VME supported\n");
   if(cpu.edx & CPUID_EDX_DE)  k_puts("DE supported\n");
   if(cpu.edx & CPUID_EDX_PSE) k_puts("PSE supported\n");
   if(cpu.edx & CPUID_EDX_TSC) k_puts("TSC supported\n");
   if(cpu.edx & CPUID_EDX_MSR) k_puts("MSR supported\n");
   if(cpu.edx & CPUID_EDX_PAE) k_puts("PAE supported\n");
   if(cpu.edx & CPUID_EDX_MCE) k_puts("MCE supported\n");
   if(cpu.edx & CPUID_EDX_CX8) k_puts("CX8 supported\n");
   if(cpu.edx & CPUID_EDX_APIC) k_puts("APIC supported\n");
   if(cpu.edx & CPUID_EDX_SEP) k_puts("SEP supported\n");
   if(cpu.edx & CPUID_EDX_MTRR) k_puts("MTRR supported\n");
   if(cpu.edx & CPUID_EDX_PGE) k_puts("PGE supported\n");
   if(cpu.edx & CPUID_EDX_MCA) k_puts("MCA supported\n");
   if(cpu.edx & CPUID_EDX_CMOV) k_puts("CMOV supported\n");
   if(cpu.edx & CPUID_EDX_PAT) k_puts("PAT supported\n");
   if(cpu.edx & CPUID_EDX_PSE36) k_puts("PSE36 supported\n");
   if(cpu.edx & CPUID_EDX_PSN) k_puts("PSN supported\n");
   if(cpu.edx & CPUID_EDX_CLF) k_puts("CLF supported\n");
   if(cpu.edx & CPUID_EDX_DTES) k_puts("DTES supported\n");
   if(cpu.edx & CPUID_EDX_ACPI) k_puts("ACPI supported\n");
   if(cpu.edx & CPUID_EDX_MMX) k_puts("MMX supported\n");
   if(cpu.edx & CPUID_EDX_FXSR) k_puts("FXSR supported\n");
   if(cpu.edx & CPUID_EDX_SSE) k_puts("SSE supported\n");
   if(cpu.edx & CPUID_EDX_SSE2) k_puts("SSE2 supported\n");
   if(cpu.edx & CPUID_EDX_SS) k_puts("SS supported\n");
   if(cpu.edx & CPUID_EDX_HTT) k_puts("HTT supported\n");
   if(cpu.edx & CPUID_EDX_TM1) k_puts("TM1 supported\n");
   if(cpu.edx & CPUID_EDX_IA64) k_puts("IA64 supported\n");
   if(cpu.edx & CPUID_EDX_PBE) k_puts("PBE supported\n");
}

void kmain64(struct multiboot_header *mbh){
   arch_init_system();

   //feature_detect();
   k_printf("%d %d", 111, 222);

   while(1) arch_halt_cpu();
}
