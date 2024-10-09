#include "CodeLib.h"

int32_t lGcd(int32_t a, int32_t b) {
  while(a && b) if (a < b) b %= a; else a %= b;
  return a + b;
}

uint32_t ulArrayMedian(uint32_t aulArr[], uint32_t ulLen) {
  uint32_t am = ulLen >> 1;
  if(ulLen && am) {
    for(int j = 0; j < ulLen; j++) {
      uint8_t bigger = 0;
      uint8_t lower = 0;
      uint32_t nb = -1;
      uint32_t nl = 0;            
      for(int i = 0; i < ulLen && (bigger <= am) && (lower <= am); i++) {
        if(aulArr[i] > aulArr[j]) {
          bigger++;
          if(nb > aulArr[i]) {
            nb = aulArr[i];
          }
        }
        else if((aulArr[i] != aulArr[j])) {
          lower++;
          if(nl < aulArr[i]) {
            nl = aulArr[i];
          }
        }
      }
      if((bigger <= am) && (lower <= am)) {
        if(!(ulLen & 1)) {
          if(bigger == am) {
            return (aulArr[j] + nb) >> 1;
          }
          if(lower == am) {
            return (aulArr[j] + nl) >> 1;
          }
        }
        return aulArr[j];
      }
    }
  }
  return aulArr[0];
}

int32_t gcd(int32_t, int32_t) __attribute__ ((alias ("lGcd")));
uint32_t array_median(uint32_t [], uint32_t) __attribute__ ((alias ("ulArrayMedian")));


