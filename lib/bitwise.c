
#include "bitwise.h"

//Routines to deal with bitfields.
//Note that these can set an arbitrary
//bit in a chunk of memory (regardless
//of the char* type).

//Set the bit in the given integer
void setBit( char *mem, int bitNum ){
   *mem = (*mem | (1 << bitNum));
}

//Clear the bit at the given index
void clearBit( char *mem, int bitNum ){
   int mask = ~(1 << bitNum);
   *mem = (*mem & mask);
}

//Return the bit value at bitNum
int getBit( char *mem, int bitNum ){
   int mask = 1 << bitNum;
   return (*mem & mask);
}


