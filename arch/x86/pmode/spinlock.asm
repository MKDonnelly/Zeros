;Synchronization primitives

; TODO add in bit test and set versions of spinlock

; Initilizes the spinlock to 0. We do
; not need a xchg here since there should
; be no race on initilizing a spinlock.
global init_spinlock
init_spinlock:
   mov eax, [esp+0x4]
   mov ecx, 0
   mov [eax], ecx
   ret

; Attempts to acquire the given lock, and
; spins until it can.
global acquire_spinlock
acquire_spinlock:
   mov eax, [esp+0x4]
   mov ecx, 1

   xchg [eax], ecx
   cmp ecx, 1

   je acquire_spinlock
   ret


global free_spinlock
free_spinlock:
   mov eax, [esp+0x4]
   mov ecx, 0

   xchg [eax], ecx
   
   ret
