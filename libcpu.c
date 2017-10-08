void stop_cpu(){
   __asm__("suspend:");
   __asm__("cli");
   __asm__("hlt");
   __asm__("jmp suspend");
}

