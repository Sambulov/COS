/**
 * @file filter_mean.h
 * @author a.bodin
 * @date 2024-01-16
 */
#ifndef FILTER_MEAN_H_
#define FILTER_MEAN_H_

#include "stdint.h"
#include "string.h"
#include "stdbool.h"

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

/* MAX LEN 65536 */
#define MAX_FILTER_MEAN_LENGH 		10

typedef uint32_t(*get_ms_time_from_fn)(uint32_t time_stamp); /* function returns difference between timestamp and current ms counters */
typedef uint32_t(*get_ms_time_fn)(void); /* function return current value ms counter */

typedef struct
{
    get_ms_time_from_fn get_ms_time_from; /* pointer fo function */
    get_ms_time_fn get_ms_time;           /* pointer fo function */
    /* Private */
    uint16_t lenght;                      /* lenght */
    uint32_t sample_time_ms;              /* sample step */
    uint8_t
        ok          : 1,                  /* init state */
        is_clear    : 1,                  /* clear flag */
        dummy       : 6;                  /* dummy */
    uint16_t cur_item_number;                    
    uint32_t time_stamp;                  
    int16_t array[MAX_FILTER_MEAN_LENGH]; 
    int32_t sum;                          
    int16_t result;
} filter_mean_t;

int16_t filter_mean (filter_mean_t *str, int16_t input);
uint8_t filter_mean_init  (filter_mean_t *str, uint16_t lenght, uint32_t sample_time_ms, get_ms_time_from_fn  get_ms_time_from_ptr, get_ms_time_fn get_ms_time_ptr);
void filter_mean_clear(filter_mean_t *str);
uint8_t filter_mean_is_full(filter_mean_t *str);
int32_t filter_mean_get(filter_mean_t *str);


#endif /* FILTER_MEAN_H_ */