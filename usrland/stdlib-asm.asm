
;The calling convention says to place all registers
;on the stack in reverse order
global do_syscall1
do_syscall1:
   mov eax, [esp+0x4]
   int 0x31
   ret

global do_syscall2
do_syscall2:
   mov eax, [esp+0x4]
   mov ebx, [esp+0x8]
   int 0x31
   ret

global do_syscall3
do_syscall3:
   mov eax, [esp+0x4]
   mov ebx, [esp+0x8]
   mov ecx, [esp+0xc]
   int 0x31
   ret

global do_syscall4
do_syscall4:
   mov eax, [esp+0x4]
   mov ebx, [esp+0x8]
   mov ecx, [esp+0xc]
   mov edx, [esp+0x10]
   int 0x31
   ret

global do_syscall5
do_syscall5:
   mov eax, [esp+0x4]
   mov ebx, [esp+0x8]
   mov ecx, [esp+0xc]
   mov edx, [esp+0x10]
   mov esi, [esp+0x14]
   int 0x31
   ret

global do_syscall6
do_syscall6:
   mov eax, [esp+0x4]
   mov ebx, [esp+0x8]
   mov ecx, [esp+0xc]
   mov edx, [esp+0x10]
   mov esi, [esp+0x14]
   mov edi, [esp+0x18]
   int 0x31
   ret
