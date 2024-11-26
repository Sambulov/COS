#ifndef MEDIAN_FILTER_H_
#define MEDIAN_FILTER_H_

#define MEDIAN_FILTER_PRV    8

typedef struct {
  uint32_t *data;
  uint16_t size;
  CL_PRIVATE(MEDIAN_FILTER_PRV);
} MedianFilter_t;

void vMedianFilterInit(MedianFilter_t *pxFilter, uint32_t ulDefaultValue, uint32_t *pulOutput);
uint8_t bMedianFilterFeed(MedianFilter_t *pxFilter, uint32_t ulValue);

/*!
  Snake notation
*/

typedef MedianFilter_t median_filter_t;

void median_filter_init(median_filter_t *filter, uint32_t default_value, uint32_t *output);
uint8_t median_filter_feed(median_filter_t *filter, uint32_t value);

#endif /* MEDIAN_FILTER_H_ */
