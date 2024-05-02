#ifndef PORT_ADC_H_
#define PORT_ADC_H_

#define EVENT_ADC_PRIVATE_SIZE  (4)
#define HDL_ADC_PRIVATE_FIELD_SIZE  (8)

// typedef enum {
//     ADC_OPERATION_MODE_SINGLE_SCAN,    /* Single SCAN, user must launched every conversion with some triger */
//     ADC_OPERATION_MODE_CONTINUOS_SCAN, /* Continuous scan, each next conversion after first triger will be do automaticly */
// } hdl_adc_operation_mode_e;

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

/* TODO: What will be if you chose a INSERTED channel? */
// /* Triger for starting conversion */
// typedef enum {
//     /* This triger can launch only regular channel conversion */
//     HDL_ADC_TRIGER_FOR_REGULAR_CHANNEL_TIMER0_CH0 = ADC_EXTTRIG_REGULAR_T0_CH0,
//     HDL_ADC_TRIGER_FOR_REGULAR_CHANNEL_TIMER0_CH1 = ADC_EXTTRIG_REGULAR_T0_CH1,
//     HDL_ADC_TRIGER_FOR_REGULAR_CHANNEL_TIMER0_CH2 = ADC_EXTTRIG_REGULAR_T0_CH2,
//     HDL_ADC_TRIGER_FOR_REGULAR_CHANNEL_TIMER2_TRG0 = ADC_EXTTRIG_REGULAR_T2_TRGO,
//     HDL_ADC_TRIGER_FOR_REGULAR_CHANNEL_TIMER14_CH0 = ADC_EXTTRIG_REGULAR_T14_CH0,
//     HDL_ADC_TRIGER_FOR_REGULAR_CHANNEL_EXTI_11 = ADC_EXTTRIG_REGULAR_EXTI_11,
//     /* This triger can launch only inserted channel conversion */
//     HDL_ADC_TRIGER_FOR_INSERTED_CHANNEL_TIMER0_TRG0 = ADC_EXTTRIG_INSERTED_T0_TRGO,
//     HDL_ADC_TRIGER_FOR_INSERTED_CHANNEL_TIMER0_CH3 = ADC_EXTTRIG_INSERTED_T0_CH3,
//     HDL_ADC_TRIGER_FOR_INSERTED_CHANNEL_TIMER2_CH3 = ADC_EXTTRIG_INSERTED_T2_CH3,
//     HDL_ADC_TRIGER_FOR_INSERTED_CHANNEL_TIMER14_TRG0 = ADC_EXTTRIG_INSERTED_T14_TRGO,
//     HDL_ADC_TRIGER_FOR_INSERTED_CHANNEL_EXTI_15 = ADC_EXTTRIG_INSERTED_EXTI_15,
//     /* This triger can launch regular and inserted channel conversion */
//     HDL_ADC_TRIGER_SOFTWARE = ADC_EXTTRIG_REGULAR_NONE,
// } hdl_adc_triger_e;

typedef struct {
    hdl_adc_channel_e channel;
    hdl_adc_channel_sample_time_e sample_time;
} hdl_adc_source_t;

typedef enum {
  HDL_ADC_DATA_ALIGN_RIGHT = ADC_DATAALIGN_RIGHT,
  HDL_ADC_DATA_ALIGN_LEFT = ADC_DATAALIGN_LEFT
} hdl_adc_data_alignment_t;

typedef struct{
    hdl_module_t module;
    hdl_adc_resolution_e resolution;
    hdl_adc_data_alignment_t data_alignment;
    uint32_t init_timeout;
    hdl_adc_source_t **sources;               /* max amount 15 */
    uint32_t *values;
    uint8_t __private[HDL_ADC_PRIVATE_FIELD_SIZE];
} hdl_adc_t;

#define hdl_adc_sources(...) ((hdl_adc_source_t *[]){__VA_ARGS__, NULL})

#define hdl_adc_src(...)  hdl_adc_sources(__VA_ARGS__),\
                          .values = (uint32_t [sizeof(hdl_adc_sources(__VA_ARGS__))/sizeof(hdl_adc_source_t *)]){}

#endif
