
void main(){
   asm volatile("movl $0, %%eax" :);
   asm volatile("movl $0x5A, %%ebx" :);
   asm volatile("int $0x31" :);
   while(1);
}
