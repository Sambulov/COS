#ifndef MEDIAN_FILTER_H_
#define MEDIAN_FILTER_H_

#define BLDL_MEDIAN_FILTER_PRV    8

typedef struct {
  uint32_t *data;
  uint16_t size;
  PRIVATE(bldl, BLDL_MEDIAN_FILTER_PRV);
} median_filter_t;

void median_filter_init(median_filter_t *filter, uint32_t default_value, uint32_t *output);
uint8_t median_filter_feed(median_filter_t *filter, uint32_t value);

#endif /* MEDIAN_FILTER_H_ */