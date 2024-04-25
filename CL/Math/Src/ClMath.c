#include "CodeLib.h"

int32_t lGcd(int32_t a, int32_t b) {
  while(a && b) if (a < b) b %= a; else a %= b;
  return a + b;
}

int32_t gcd(int32_t, int32_t) __attribute__ ((alias ("lGcd")));
