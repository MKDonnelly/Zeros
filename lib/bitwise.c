
#include "bitwise.h"

//Routines to deal with bitfields.
//Note that these can set an arbitrary
//bit in a chunk of memory (regardless
//of the char* type).

//Set the bit in the given integer
void setBit( void *mem, int bitNum ){
   *(char*)mem = (*(char*)mem | (1 << bitNum));
}

/*void setBit( char *mem, int bitNum ){
   *mem = (*mem | (1 << bitNum));
}*/

//Clear the bit at the given index
void clearBit( void *mem, int bitNum ){
   int mask = ~(1 << bitNum);
   *(char*)mem = (*(char*)mem & mask);
}
/*void clearBit( char *mem, int bitNum ){
   int mask = ~(1 << bitNum);
   *mem = (*mem & mask);
}*/

//Return the bit value at bitNum
int getBit( void *mem, int bitNum ){
   int mask = 1 << bitNum;
   return (*(char*)mem & mask);
}
/*int getBit( char *mem, int bitNum ){
   int mask = 1 << bitNum;
   return (*mem & mask);
}*/


