#ifndef PORT_ADC_H_
#define PORT_ADC_H_

// #define EVENT_ADC_PRIVATE_SIZE  (4)
// #define ADC_PRIVATE_FIELD_SIZE  (8)

// typedef enum {
//     ADC_OPERATION_MODE_SINGLE_SCAN,    /* Single SCAN, user must launched every conversion with some triger */
//     ADC_OPERATION_MODE_CONTINUOS_SCAN, /* Continuous scan, each next conversion after first triger will be do automaticly */
// } hdl_adc_operation_mode_e;

// typedef enum {
//     HDL_ADC_CHANNEL_0 = ADC_CHANNEL_0,
//     HDL_ADC_CHANNEL_1 = ADC_CHANNEL_1,
//     HDL_ADC_CHANNEL_2 = ADC_CHANNEL_2,
//     HDL_ADC_CHANNEL_3 = ADC_CHANNEL_3,
//     HDL_ADC_CHANNEL_4 = ADC_CHANNEL_4,
//     HDL_ADC_CHANNEL_5 = ADC_CHANNEL_5,
//     HDL_ADC_CHANNEL_6 = ADC_CHANNEL_6,
//     HDL_ADC_CHANNEL_7 = ADC_CHANNEL_7,
//     HDL_ADC_CHANNEL_8 = ADC_CHANNEL_8,
//     HDL_ADC_CHANNEL_9 = ADC_CHANNEL_9,
//     HDL_ADC_CHANNEL_16 = ADC_CHANNEL_16,
//     HDL_ADC_CHANNEL_17 = ADC_CHANNEL_17,
// } hdl_adc_channel_e;

// /* Time sampling for channel  */
// typedef enum{
//     HDL_ADC_CHANNEL_SAMPLE_TIME_1P5 = ADC_SAMPLETIME_1POINT5,     /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 1.5 CK_ADC cycles*/
//     HDL_ADC_CHANNEL_SAMPLE_TIME_7P5 = ADC_SAMPLETIME_7POINT5,     /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 7.5 CK_ADC cycles*/
//     HDL_ADC_CHANNEL_SAMPLE_TIME_13P5 = ADC_SAMPLETIME_13POINT5,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 13.5 CK_ADC cycles*/
//     HDL_ADC_CHANNEL_SAMPLE_TIME_28P5 = ADC_SAMPLETIME_28POINT5,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 28.5 CK_ADC cycles*/
//     HDL_ADC_CHANNEL_SAMPLE_TIME_41P5 = ADC_SAMPLETIME_41POINT5,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 41.5 CK_ADC cycles*/
//     HDL_ADC_CHANNEL_SAMPLE_TIME_55P5 = ADC_SAMPLETIME_55POINT5,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 55.5 CK_ADC cycles*/
//     HDL_ADC_CHANNEL_SAMPLE_TIME_71P5 = ADC_SAMPLETIME_71POINT5,   /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 71.5 CK_ADC cycles*/
//     HDL_ADC_CHANNEL_SAMPLE_TIME_239P5 = ADC_SAMPLETIME_239POINT5, /* 12.5 CK_ADC cycles (const value for 12-bit adc conversion) + 239.5 CK_ADC cycles*/
// } hdl_adc_channel_sample_time_e;

// typedef enum {
//     /* This triger can launch only regular channel conversion */
//     /* ADC0, ADC1 */
//     HDL_ADC0_1_EXTTRIG_REGULAR_T0_CH0 = ADC0_1_EXTTRIG_REGULAR_T0_CH0,
//     HDL_ADC0_1_EXTTRIG_REGULAR_T0_CH1 = ADC0_1_EXTTRIG_REGULAR_T0_CH1,
//     HDL_ADC0_1_EXTTRIG_REGULAR_T0_CH2 = ADC0_1_EXTTRIG_REGULAR_T0_CH2,
//     HDL_ADC0_1_EXTTRIG_REGULAR_T1_CH1 = ADC0_1_EXTTRIG_REGULAR_T1_CH1,
//     HDL_ADC0_1_EXTTRIG_REGULAR_T2_TRGO = ADC0_1_EXTTRIG_REGULAR_T2_TRGO,
//     HDL_ADC0_1_EXTTRIG_REGULAR_T3_CH3 = ADC0_1_EXTTRIG_REGULAR_T3_CH3,
//     HDL_ADC0_1_EXTTRIG_REGULAR_T7_TRGO = ADC0_1_EXTTRIG_REGULAR_T7_TRGO,
//     HDL_ADC0_1_EXTTRIG_REGULAR_EXTI_11 = ADC0_1_EXTTRIG_REGULAR_EXTI_11,
//     /* ADC2 */
//     HDL_ADC2_EXTTRIG_REGULAR_T2_CH0 = ADC2_EXTTRIG_REGULAR_T2_CH0,
//     HDL_ADC2_EXTTRIG_REGULAR_T1_CH2 = ADC2_EXTTRIG_REGULAR_T1_CH2,
//     HDL_ADC2_EXTTRIG_REGULAR_T0_CH2 = ADC2_EXTTRIG_REGULAR_T0_CH2,
//     HDL_ADC2_EXTTRIG_REGULAR_T7_CH0 = ADC2_EXTTRIG_REGULAR_T7_CH0,
//     HDL_ADC2_EXTTRIG_REGULAR_T7_TRGO = ADC2_EXTTRIG_REGULAR_T7_TRGO,
//     HDL_ADC2_EXTTRIG_REGULAR_T4_CH0 = ADC2_EXTTRIG_REGULAR_T4_CH0,
//     HDL_ADC2_EXTTRIG_REGULAR_T4_CH2 = ADC2_EXTTRIG_REGULAR_T4_CH2,
//     /* This triger can launch only inserted channel conversion */
//     /* ADC0, ADC1 */
//     HDL_ADC0_1_EXTTRIG_INSERTED_T0_TRGO = ADC0_1_EXTTRIG_INSERTED_T0_TRGO,
//     HDL_ADC0_1_EXTTRIG_INSERTED_T0_CH3 = ADC0_1_EXTTRIG_INSERTED_T0_CH3,
//     HDL_ADC0_1_EXTTRIG_INSERTED_T1_TRGO = ADC0_1_EXTTRIG_INSERTED_T1_TRGO,
//     HDL_ADC0_1_EXTTRIG_INSERTED_T1_CH0 = ADC0_1_EXTTRIG_INSERTED_T1_CH0,
//     HDL_ADC0_1_EXTTRIG_INSERTED_T2_CH3 = ADC0_1_EXTTRIG_INSERTED_T2_CH3,
//     HDL_ADC0_1_EXTTRIG_INSERTED_T3_TRGO = ADC0_1_EXTTRIG_INSERTED_T3_TRGO,
//     HDL_ADC0_1_EXTTRIG_INSERTED_EXTI_15 = ADC0_1_EXTTRIG_INSERTED_EXTI_15,
//     HDL_ADC0_1_EXTTRIG_INSERTED_T7_CH3 = ADC0_1_EXTTRIG_INSERTED_T7_CH3,
//     /* ADC2 */
//     HDL_ADC2_EXTTRIG_INSERTED_T0_TRGO = ADC2_EXTTRIG_INSERTED_T0_TRGO,
//     HDL_ADC2_EXTTRIG_INSERTED_T0_CH3 = ADC2_EXTTRIG_INSERTED_T0_CH3,
//     HDL_ADC2_EXTTRIG_INSERTED_T3_CH2 = ADC2_EXTTRIG_INSERTED_T3_CH2,
//     HDL_ADC2_EXTTRIG_INSERTED_T7_CH1 = ADC2_EXTTRIG_INSERTED_T7_CH1,
//     HDL_ADC2_EXTTRIG_INSERTED_T7_CH3 = ADC2_EXTTRIG_INSERTED_T7_CH3,
//     HDL_ADC2_EXTTRIG_INSERTED_T4_TRGO = ADC2_EXTTRIG_INSERTED_T4_TRGO,
//     HDL_ADC2_EXTTRIG_INSERTED_T4_CH3 = ADC2_EXTTRIG_INSERTED_T4_CH3,
//     /* This triger can launch regular and inserted channel conversion */
//     HDL_ADC0_1_2_REGULAR_TRIGER_SOFTWARE = ADC0_1_2_EXTTRIG_REGULAR_NONE,
//     HDL_ADC0_1_2_INSERTED_TRIGER_SOFTWARE = ADC0_1_2_EXTTRIG_INSERTED_NONE
// } hdl_adc_triger_e;

// typedef struct {
//     hdl_adc_channel_e channel_number;
//     hdl_adc_channel_sample_time_e channel_sample_time;
// } hdl_adc_source_t;

// typedef struct {
//     uint8_t __private[EVENT_ADC_PRIVATE_SIZE];
//     event_handler_t conversion_finished;
//     event_handler_t analog_watchdog_triger;
// } hdl_adc_event_t;

// typedef enum {
//   HDL_ADC_DATA_ALIGN_RIGHT = ADC_DATAALIGN_RIGHT,
//   HDL_ADC_DATA_ALIGN_LEFT = ADC_DATAALIGN_LEFT
// } hdl_adc_data_alignment_t;

// typedef struct{
//     hdl_module_t module;
//     hdl_dma_channel_t dma_channel;
//     hdl_adc_triger_e start_triger;
//     hdl_adc_operation_mode_e mode;
//     //hdl_adc_resolution_e resolution;
//     hdl_adc_data_alignment_t data_alignment;
//     uint32_t init_timeout;
//     hdl_adc_source_t **sources;               /* max amount 15 */
// } hdl_adc_t;


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

// typedef enum {
//     HDL_ADC_RESOLUTION_12BIT = ADC_RESOLUTIOL,
//     HDL_ADC_RESOLUTION_10BIT = ADC_RESOLUTION_10B,
//     HDL_ADC_RESOLUTION_8BIT = ADC_RESOLUTION_8B,
//     HDL_ADC_RESOLUTION_6BIT = ADC_RESOLUTION_6B,
// } hdl_adc_resolution_e;


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
    hdl_adc_data_alignment_t data_alignment;
    uint32_t init_timeout;
    hdl_adc_source_t **sources;               /* max amount 15 */
    uint32_t *values;
    PRIVATE(HDL_ADC_PRIVATE_FIELD_SIZE);
} hdl_adc_t;

#define hdl_adc_sources(...) ((hdl_adc_source_t *[]){__VA_ARGS__, NULL})

#define hdl_adc_src(...)  hdl_adc_sources(__VA_ARGS__),\
                          .values = (uint32_t [sizeof(hdl_adc_sources(__VA_ARGS__))/sizeof(hdl_adc_source_t *)]){}



#endif