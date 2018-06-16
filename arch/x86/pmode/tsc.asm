;read time stamp counter to
;generate entropy for the system

global arch_generate_entropy
arch_generate_entropy:
   rdtsc
   ret 
