; Jump to the specified code. Accepts
; A single argument of thread_context_t*
; to jump to 
global arch_jump_to_thread
arch_jump_to_thread:
   mov esp, [esp+4]
   popa

   ; In our interrupt code, we push an error
   ; code and interrupt number before pushing
   ; the GPRs. This removes them.
   add esp, 8
   ret
