;Synchronization primitives

global spinlock_init
spinlock_init:
   mov rax, [rsp+0x08]
   mov rcx, 0
   mov [rax], rcx
   ret

global spinlock_acquire
spinlock_acquire:
   mov rax, [rsp+0x8]
   mov rcx, 1

   xchg [rax], rcx
   cmp rcx, 1

   je spinlock_acquire
   ret

global spinlock_release
spinlock_release:
   mov rax, [rsp+0x8]
   mov rcx, 0

   xchg [rax], rcx

   ret
   
