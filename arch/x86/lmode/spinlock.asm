;Synchronization primitives

global spinlock_init
spinlock_init:
   ;caller put address in rdi
   mov rcx, 0
   mov [rdi], rcx
   ret

global spinlock_acquire
spinlock_acquire:
   ;caller put address in rdi
   mov rcx, 1

   xchg [rdi], rcx
   cmp rcx, 1

   je spinlock_acquire
   ret

global spinlock_release
spinlock_release:
   ;caller puts address in rdi
   mov rcx, 0
   xchg [rdi], rcx
   ret
