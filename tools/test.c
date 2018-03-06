
int syscall_putchar(char);

void main(){
   //syscall_putchar('Z');
//   asm volatile("int $100");
   while(1);
}

int syscall_putchar(char arg1) { 
   int syscall_return;   
   asm volatile("int $0x50" : "=a" (syscall_return) : "0"(0), "b"((int)arg1)); 
   return syscall_return; 
}
