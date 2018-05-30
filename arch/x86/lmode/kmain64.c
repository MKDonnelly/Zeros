#include "gdt.h"
void kmain64(){
   char *ptr = (char*)0xb8000;
   *(ptr) = 'Y';
   *(ptr + 2) = 'e';
   *(ptr + 4) = 'l';
   *(ptr + 6) = 'l';
   *(ptr + 8) = 'o';
}

