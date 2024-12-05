#include "app.h"
#include "CodeLib.h"

typedef struct {
  uint32_t *ulData;
  uint16_t usSize;
  struct {
    uint32_t *pulOutput;
    uint16_t usAmount;
    uint16_t usIndex;
  } xPrivate;
} MedianFilterPrv_t;

HDL_ASSERRT_STRUCTURE_CAST(MedianFilterPrv_t, MedianFilter_t, MEDIAN_FILTER_PRV, MedianFilter.h);

void vMedianFilterInit(MedianFilter_t *pxFilter, uint32_t ulDefaultValue, uint32_t *pulOutput) {
  if(pxFilter == NULL) return;
  MedianFilterPrv_t *pxFlt = (MedianFilterPrv_t *)pxFilter;
  pxFlt->xPrivate.usIndex = 0;
  pxFlt->xPrivate.usAmount = 0;
  pxFlt->xPrivate.pulOutput = pulOutput;
  if(pxFlt->xPrivate.pulOutput != NULL) *(pxFlt->xPrivate.pulOutput) = ulDefaultValue;
}

static uint32_t _bFilterFindMedian(MedianFilterPrv_t *pxFlt) {
  uint32_t tmp;
  uint8_t inserted = 0;
  uint32_t median = pxFlt->ulData[pxFlt->xPrivate.usIndex];
  while(1) {
    uint32_t bigger = 0;
    uint32_t lower = 0;
    uint32_t next_bigger = median;
    uint32_t next_lower = median;
    for(int i = 0; i < pxFlt->xPrivate.usAmount; i++) {
      if(pxFlt->ulData[i] > median) {
        if((next_bigger == median) || (next_bigger > pxFlt->ulData[i])) {
          next_bigger = pxFlt->ulData[i];
        }
        bigger++;
      } else if (pxFlt->ulData[i] < median) {
        if((next_lower == median) || (next_lower < pxFlt->ulData[i])) {
          next_lower = pxFlt->ulData[i];
        }
        lower++;
      }
    }
    if((pxFlt->xPrivate.usAmount >= (bigger << 1)) && (pxFlt->xPrivate.usAmount >= (lower << 1))) {
      if(pxFlt->xPrivate.usAmount & 1) return median;
      uint32_t add = next_lower;
      if(lower < (pxFlt->xPrivate.usAmount >> 1)) add = median;
      if (bigger > lower) {
        add = next_bigger;
        if(bigger < (pxFlt->xPrivate.usAmount >> 1)) add = median;
      }
      return ((add & median) & 1) + (median >> 1) + (add >> 1);
    }
    median = (bigger > lower)? next_bigger: next_lower;
  }
}

uint8_t bMedianFilterFeed(MedianFilter_t *pxFilter, uint32_t value) {
  if(pxFilter == NULL) return 0;
  MedianFilterPrv_t *pxFlt = (MedianFilterPrv_t *)pxFilter;
  pxFlt->ulData[pxFlt->xPrivate.usIndex] = value;
  if(pxFlt->xPrivate.usAmount < pxFlt->usSize) pxFlt->xPrivate.usAmount++;
  if(pxFlt->xPrivate.pulOutput != NULL) *(pxFlt->xPrivate.pulOutput) = _bFilterFindMedian(pxFlt);
  pxFlt->xPrivate.usIndex++;
  if(pxFlt->xPrivate.usIndex >= pxFlt->usSize) {
    pxFlt->xPrivate.usIndex = 0;
    return 1;
  }
  return 0;
}

void median_filter_init(median_filter_t *filter, uint32_t default_value, uint32_t *output)\
                                                      __attribute__ ((alias ("vMedianFilterInit")));
uint8_t median_filter_feed(median_filter_t *filter, uint32_t value)\
                                                      __attribute__ ((alias ("bMedianFilterFeed")));
