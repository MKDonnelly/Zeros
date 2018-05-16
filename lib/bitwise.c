
#include <lib/bitwise.h>

//Routines to deal with bitfields.
//Note that these can set an arbitrary
//bit in a chunk of memory (regardless
//of the char* type).

void bit_set( void *mem, int bitNum ){
   //Due to how dereferencing works, we
   //need to find the index and offset
   //of the bit in memory (with regard
   //to a char type).
   int index = bitNum / CHAR_BITS;
   int offset = bitNum % CHAR_BITS;

   //In this ugly expression, we are casting
   //the void* of the first parameter into
   //something that we can use, a char*.
   //We then access that char* as an array
   //and set the specified bit.
   ( (char*)mem )[index] |= (0x1 << offset);
}

void bit_clear( void *mem, int bitNum ){
   //Due to how dereferencing works, we
   //need to find the index and offset
   //of the bit in memory (with regard
   //to a char type).
   int index = bitNum / CHAR_BITS;
   int offset = bitNum % CHAR_BITS;

   //In this ugly expression, we are casting
   //the void* of the first parameter into
   //something that we can use, a char*.
   //We then access that char* as an array
   //and clear the specified bit.
   ( (char*)mem )[index] &= ~( 0x1 << offset );
}

//We will return a char as it is the smallest
//natural data type. We really only get a
//1 or 0.
uint8_t bit_get( void *mem, int bitNum ){
   //Due to how dereferencing works, we
   //need to find the index and offset
   //of the bit in memory (with regard
   //to a char type).
   int index = bitNum / CHAR_BITS;
   int offset = bitNum % CHAR_BITS;

   //In this ugly expression, we are casting
   //the void* of the first parameter into
   //something that we can use, a char*.
   //We then get the specified char at the index
   //and select the given bit using the &
   char val = ( ((char*)mem)[index] & (0x1 << offset) );
   return (val ? 1 : 0);
}
