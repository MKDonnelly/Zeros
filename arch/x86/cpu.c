
//Various routines to manage the cpu

void arch_stop_cpu(){
   asm("suspend:");
   asm("   cli");
   asm("   hlt");
   asm("   jmp suspend");
}

void arch_disable_ints(){
   asm volatile("cli");
}

void arch_enable_ints(){
   asm volatile("sti");
}

