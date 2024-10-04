#include "app.h"
#include "CodeLib.h"

typedef struct {
  uint32_t *data;
  uint16_t size;
  struct {
    uint32_t *output;
    uint16_t amount;
    uint16_t index;
  } private;
} median_filter_private_t;

HDL_ASSERRT_STRUCTURE_CAST(median_filter_private_t, median_filter_t, BLDL_MEDIAN_FILTER_PRV, median_filter.h);

void median_filter_init(median_filter_t *filter, uint32_t default_value, uint32_t *output) {
  if(filter == NULL) return;
  median_filter_private_t *flt = (median_filter_private_t *)filter;
  flt->private.index = 0;
  flt->private.amount = 0;
  flt->private.output = output;
  if(flt->private.output != NULL) *(flt->private.output) = default_value;
}

static uint32_t _filter_find_median(median_filter_private_t *flt) {
  uint32_t tmp;
  uint8_t inserted = 0;
  uint32_t median = flt->data[flt->private.index];
  while(1) {
    uint32_t bigger = 0;
    uint32_t lower = 0;
    uint32_t next_bigger = median;
    uint32_t next_lower = median;

    for(int i = 0; i < flt->private.amount; i++) {
      if(flt->data[i] > median) {
        if((next_bigger == median) || (next_bigger > flt->data[i])) {
          next_bigger = flt->data[i];
        }
        bigger++;
      } else if (flt->data[i] < median) {
        if((next_lower == median) || (next_lower < flt->data[i])) {
          next_lower = flt->data[i];
        }
        lower++;
      }
    }
    bigger <<= 1;
    lower <<= 1;
    if((flt->private.amount >= bigger) && (flt->private.amount >= lower)) {
      if(flt->private.amount & 1)
        return median;
      if (bigger > lower) {
        return (median >> 1) + (next_bigger >> 1) + ((next_bigger & 1) & (median & 1));
      }
      return (median >> 1) + (next_lower >> 1) + ((next_lower & 1) & (median & 1));
    }
    median = (bigger > lower)? next_bigger: next_lower;
  }
}

uint8_t median_filter_feed(median_filter_t *filter, uint32_t value) {
  if(filter == NULL) return 0;
  median_filter_private_t *flt = (median_filter_private_t *)filter;
  flt->data[flt->private.index] = value;
  if(flt->private.amount < flt->size) flt->private.amount++;
  if(flt->private.output != NULL) *(flt->private.output) = _filter_find_median(flt);
  flt->private.index++;
  if(flt->private.index >= flt->size) {
    flt->private.index = 0;
    return 1;
  }
  return 0;
}
