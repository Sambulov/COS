#ifndef PORT_ADC_H_
#define PORT_ADC_H_

#include "hdl_adc.h"

#define HDL_ADC_VAR_SIZE  56

typedef enum {
    HDL_ADC_CHANNEL_0 = ADC_CHANNEL_0,
    HDL_ADC_CHANNEL_1 = ADC_CHANNEL_1,
    HDL_ADC_CHANNEL_2 = ADC_CHANNEL_2,
    HDL_ADC_CHANNEL_3 = ADC_CHANNEL_3,
    HDL_ADC_CHANNEL_4 = ADC_CHANNEL_4,
    HDL_ADC_CHANNEL_5 = ADC_CHANNEL_5,
    HDL_ADC_CHANNEL_6 = ADC_CHANNEL_6,
    HDL_ADC_CHANNEL_7 = ADC_CHANNEL_7,
    HDL_ADC_CHANNEL_8 = ADC_CHANNEL_8,
    HDL_ADC_CHANNEL_9 = ADC_CHANNEL_9,
    HDL_ADC_CHANNEL_16 = ADC_CHANNEL_16,
    HDL_ADC_CHANNEL_17 = ADC_CHANNEL_17,
} hdl_adc_channel_e;

typedef enum{
    HDL_ADC_SAMPLETIME_3   = ADC_SAMPLETIME_3,
    HDL_ADC_SAMPLETIME_15  = ADC_SAMPLETIME_15,
    HDL_ADC_SAMPLETIME_28  = ADC_SAMPLETIME_28,
    HDL_ADC_SAMPLETIME_56  = ADC_SAMPLETIME_56,
    HDL_ADC_SAMPLETIME_84  = ADC_SAMPLETIME_84,
    HDL_ADC_SAMPLETIME_112 = ADC_SAMPLETIME_112,
    HDL_ADC_SAMPLETIME_144 = ADC_SAMPLETIME_144,
    HDL_ADC_SAMPLETIME_480 = ADC_SAMPLETIME_480
} hdl_adc_channel_sample_time_e;

typedef enum {
    HDL_ADC_RESOLUTION_12BIT = ADC_RESOLUTION_12B,
    HDL_ADC_RESOLUTION_10BIT = ADC_RESOLUTION_10B,
    HDL_ADC_RESOLUTION_8BIT = ADC_RESOLUTION_8B,
    HDL_ADC_RESOLUTION_6BIT = ADC_RESOLUTION_6B,
} hdl_adc_resolution_e;


typedef struct {
    hdl_adc_channel_e channel;
    hdl_adc_channel_sample_time_e sample_time;
} hdl_adc_source_t;

typedef enum {
  HDL_ADC_DATA_ALIGN_RIGHT = ADC_DATAALIGN_RIGHT,
  HDL_ADC_DATA_ALIGN_LEFT = ADC_DATAALIGN_LEFT
} hdl_adc_data_alignment_t;

typedef struct{
  uint32_t phy;
  hdl_interrupt_t *adc_interrupt;
  hdl_adc_resolution_e resolution;
  hdl_adc_data_alignment_t data_alignment;
  uint32_t init_timeout;
  const hdl_adc_source_t * const *sources;               /* max amount 15 */
  uint32_t *values;
} hdl_adc_config_t;

#define hdl_adc_sources(...) ((const hdl_adc_source_t const *[]){__VA_ARGS__, NULL})

#define hdl_adc_src(...)  hdl_adc_sources(__VA_ARGS__),\
                          .values = (uint32_t [sizeof(hdl_adc_sources(__VA_ARGS__))/sizeof(hdl_adc_source_t *)]){}

/* depends on:
  hdl_clock_t
  hdl_time_counter_t
  hdl_dma_channel_t
  hdl_interrupt_controller_t
 */
hdl_module_new_t(hdl_adc_mcu_t, HDL_ADC_VAR_SIZE, hdl_adc_config_t, hdl_adc_iface_t);

extern const hdl_adc_iface_t hdl_adc_iface;

#endif
