#include "log2i.h"

// Computes the smallest power of 2 greater than the parameter
uint32_t roundUpPower2(uint32_t x)
{
  // efficient branchless code extracted from "Hacker's Delight" by
  // Henry S. Warren, Jr.

  x = x - 1;
  x = x | (x >>  1);
  x = x | (x >>  2);
  x = x | (x >>  4);
  x = x | (x >>  8);
  x = x | (x >> 16);
  return x + 1;
}

// Computes log2(n) after rounding up n to the closest power of 2
short log2i(uint32_t n)
{
  unsigned long m = 1;
  unsigned long i = 0;

  if (n==1)
    return 0;

  n= roundUpPower2(n);
  //assume integer power of 2
  assert((n & (n - 1)) == 0);

  while(m < n) {
    i++;
    m <<= 1;
  }

  return i;
}

