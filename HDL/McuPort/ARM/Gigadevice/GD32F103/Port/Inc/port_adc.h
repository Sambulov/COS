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

/* Time sampling for channel  */
typedef enum{
    HDL_ADC_CHANNEL_SAMPLE_TIME_1P5 = ADC_SAMPLETIME_1POINT5,     /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 1.5 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_7P5 = ADC_SAMPLETIME_7POINT5,     /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 7.5 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_13P5 = ADC_SAMPLETIME_13POINT5,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 13.5 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_28P5 = ADC_SAMPLETIME_28POINT5,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 28.5 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_41P5 = ADC_SAMPLETIME_41POINT5,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 41.5 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_55P5 = ADC_SAMPLETIME_55POINT5,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 55.5 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_71P5 = ADC_SAMPLETIME_71POINT5,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 71.5 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_239P5 = ADC_SAMPLETIME_239POINT5, /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 239.5 CK_ADC cycles*/
} hdl_adc_channel_sample_time_e;

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
  rcu_periph_enum rcu;
  hdl_interrupt_t *adc_interrupt;
  hdl_adc_data_alignment_t data_alignment;
  uint32_t init_timeout;
  const hdl_adc_source_t * const *sources;               /* max amount 15 */
  uint32_t *values;
} hdl_adc_config_t;

#define hdl_adc_sources(...) ((const hdl_adc_source_t * const []){__VA_ARGS__, NULL})

#define hdl_adc_src(...)  hdl_adc_sources(__VA_ARGS__),\
                          .values = (uint32_t [sizeof(hdl_adc_sources(__VA_ARGS__))/sizeof(hdl_adc_source_t *)]){}

/* depends on
  hdl_clock_t
  hdl_time_counter_t
  hdl_dma_channel_t
  hdl_interrupt_controller_t
*/

hdl_module_new_t(hdl_adc_mcu_t, HDL_ADC_VAR_SIZE, hdl_adc_config_t, hdl_adc_iface_t);

extern const hdl_adc_iface_t hdl_adc_iface;

#endif