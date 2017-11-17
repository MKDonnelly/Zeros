//Various generic functions to modify memory
//

void memcpy( char *dest, char *src, int amount){
   for(int i = 0; i < amount; i++)
      dest[i] = src[i];
}

void memset( char *dest, int amount, int val){
   for(int i = 0; i < amount; i++)
      dest[i] = val;
}


