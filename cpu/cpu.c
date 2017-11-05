
//Various routines to manage the cpu

void stop_cpu(){
   asm("suspend:");
   asm("   cli");
   asm("   hlt");
   asm("   jmp suspend");
}

void disable_ints(){
   asm volatile("cli");
}

void enable_ints(){
   asm volatile("sti");
}


