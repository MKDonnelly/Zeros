
#include <lib/bcd.h>

//Every 4 binary digits of the bcd number
//equal the corresponding digit in decimal.
int8_t bcdtobin( int8_t bcdNumber ){
   char lowerBits = bcdNumber & 0x0F;
   char upperBits = bcdNumber >> 4;

   int decimalValue = 0;
   decimalValue += upperBits * 10;
   decimalValue += lowerBits;
   return decimalValue;
}


