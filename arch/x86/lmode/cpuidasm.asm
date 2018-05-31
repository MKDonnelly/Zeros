
;Returns non-zero if cpuid is supported
global cpuid_avail
cpuid_avail:
   ; Put eflags into eax
   pushfq
   pop rax

   ;copy eflags to rcx for comparing later
   mov rcx, rax

   ;flip the ID bit
   xor rax, 1<<21

   ;copy eax to flags via the stack
   push rax
   popfq

   ;copy eflags back. If the bit was flipped,
   ;then cpuid is supported
   pushfq
   pop rax

   ;restore eflags
   push rcx
   popfq

   ;compare eax and ecx. If they are not equal,
   ;eflags is supported
   xor rax, rcx
   ret
