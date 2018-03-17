
void kmain_64(){
   char *ptr = (char*)0xb8000;
   *ptr = 'Z';
   *(ptr + 2) = 'Z';
   *(ptr + 4) = 'Z';
   while(1);
}

