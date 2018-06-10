;Synchronization primitives

global spinlock_init
spinlock_init:
   ;caller put address in rax
   mov rcx, 0
   mov [rax], rcx
   ret

global spinlock_acquire
spinlock_acquire:
   ;caller put address in rax
   mov rcx, 1
   ;save address in rax for next iter
   mov rdi, rax 

   xchg [rax], rcx
   cmp rcx, 1

   mov rax, rdi

   je spinlock_acquire
   ret

global spinlock_release
spinlock_release:
   ;caller puts address in rax
   mov rcx, 0

   xchg [rax], rcx

   ret
   
