; Functions for enabling/disabling features of
; the cpu and other related stuff.

;Useful when debugging
global arch_stop_cpu
arch_stop_cpu:
 stop:
   cli
   hlt
   jmp stop

global arch_halt_cpu
arch_halt_cpu:
   hlt
   ret

global arch_enable_ints
arch_enable_ints:
   sti
   ret

global arch_disable_ints
arch_disable_ints:
   cli
   ret

