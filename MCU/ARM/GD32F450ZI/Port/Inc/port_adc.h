#ifndef PORT_ADC_H_
#define PORT_ADC_H_

#define EVENT_ADC_PRIVATE_SIZE  (4)
#define ADC_PRIVATE_FIELD_SIZE  (8)

typedef enum {
    ADC_OPERATION_MODE_SINGLE_SCAN,    /* Single SCAN, user must launched every conversion with some triger */
    ADC_OPERATION_MODE_CONTINUOS_SCAN, /* Continuous scan, each next conversion after first triger will be do automaticly */
} hdl_adc_operation_mode_e;

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
    HDL_ADC_CHANNEL_SAMPLE_TIME_3 = ADC_SAMPLETIME_3,     /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 3 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_15 = ADC_SAMPLETIME_15,     /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 15 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_28 = ADC_SAMPLETIME_28,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 28 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_56 = ADC_SAMPLETIME_56,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 56 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_84 = ADC_SAMPLETIME_84,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 84 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_112 = ADC_SAMPLETIME_112,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 112 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_144 = ADC_SAMPLETIME_144,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 144 CK_ADC cycles*/
    HDL_ADC_CHANNEL_SAMPLE_TIME_480 = ADC_SAMPLETIME_480, /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 480 CK_ADC cycles*/
} hdl_adc_channel_sample_time_e;

typedef enum {
    HDL_ADC_RESOLUTION_12BIT = ADC_RESOLUTION_12B,
    HDL_ADC_RESOLUTION_10BIT = ADC_RESOLUTION_10B,
    HDL_ADC_RESOLUTION_8BIT = ADC_RESOLUTION_8B,
    HDL_ADC_RESOLUTION_6BIT = ADC_RESOLUTION_6B,
} hdl_adc_resolution_e;

/* TODO: What will be if you chose a INSERTED channel? */
/* Triger for starting conversion */
typedef enum {
    /* This triger can launch only regular channel conversion */
    HDL_ADC_TRIGER_FOR_REGULAR_CHANNEL_TIMER0_CH0 = 0,
} hdl_adc_triger_e;

typedef struct {
    hdl_adc_channel_e channel_number;
    hdl_adc_channel_sample_time_e channel_sample_time;
} hdl_adc_source_t;

typedef struct {
    uint8_t __private[EVENT_ADC_PRIVATE_SIZE];
    event_handler_t conversion_finished;
    event_handler_t analog_watchdog_triger;
} hdl_adc_event_t;

typedef enum {
  HDL_ADC_DATA_ALIGN_RIGHT = ADC_DATAALIGN_RIGHT,
  HDL_ADC_DATA_ALIGN_LEFT = ADC_DATAALIGN_LEFT
} hdl_adc_data_alignment_t;

typedef struct{
    hdl_module_t module;
    uint8_t __private[ADC_PRIVATE_FIELD_SIZE];
    hdl_dma_channel_t dma_channel;
    hdl_adc_triger_e start_triger;
    hdl_adc_operation_mode_e mode;
    hdl_adc_resolution_e resolution;
    hdl_adc_data_alignment_t data_alignment;
    uint32_t init_timeout;
    hdl_adc_source_t **sources;               /* max amount 15 */
} hdl_adc_t;

#endif