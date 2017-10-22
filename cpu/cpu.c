
//Various routines to manage the cpu

void stop_cpu(){
   __asm__("suspend:");
   __asm__("cli");
   __asm__("hlt");
   __asm__("jmp suspend");
}

void disable_ints(){
   __asm__("cli\n\t");
}

void enable_ints(){
   __asm__("sti\n\t");
}

/*
void isr_start(){
   __asm__("pusha");
   __asm__("mov ax, ds");
   __asm__("
}*/

