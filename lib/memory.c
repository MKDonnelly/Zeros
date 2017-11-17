//Various generic functions to modify memory
//

void memcpy( void *dest, void *src, int amount){
   for(int i = 0; i < amount; i++)
      ((char*)dest)[i] = ((char*)src)[i];
}

//Overwrite *dest with amount bytes of value val
void memset( void *dest, int amount, int val){
   for(int i = 0; i < amount; i++)
      ((char*)dest)[i] = 0;
}

