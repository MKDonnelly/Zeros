
//Various routines to manage the cpu

void stop_cpu(){
   asm("suspend:");
   asm("cli");
   asm("hlt");
   asm("jmp suspend");
}

void disable_ints(){
   asm("cli\n\t");
}

void enable_ints(){
   asm("sti\n\t");
}


