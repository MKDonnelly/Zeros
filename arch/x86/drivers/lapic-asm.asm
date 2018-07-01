global apic_test
apic_test:
   mov ecx, 0x1B
   rdmsr
   ret
