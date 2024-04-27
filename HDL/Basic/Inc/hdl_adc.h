#ifndef HDL_ADC_H_
#define HDL_ADC_H_

#include "port_adc.h"

/* 
*   TODO: oversampling don`t support, discontinuos operation mode don`t support, single operation don`t support,
*   conversion result threshold monitor function don`t support 
*/
typedef enum {
    HDL_ADC_STATUS_INIT_FAILED = 0,
    HDL_ADC_STATUS_WAITING_START_TRIGGER,
    HDL_ADC_STATUS_DATA_READY,
    HDL_ADC_STATUS_ONGOING,
} hdl_adc_status_e;

#define hdl_adc_sources(...) ((hdl_adc_source_t *[]){__VA_ARGS__, NULL})
//#define hdl_adc_sources(...) .source = (hdl_adc_source_t *[]){__VA_ARGS__, NULL}, .buf = (uint16_t [sizeof({(hdl_adc_source_t *[]){__VA_ARGS__, NULL}})/sizeof(hdl_adc_source_t *)]){}

#define hdl_adc_src(...)  .sources = hdl_adc_sources(__VA_ARGS__),\
                          .buf = (uint16_t [sizeof(hdl_adc_sources(__VA_ARGS__))/sizeof(hdl_adc_source_t *)]){}

/* Initialization */
hdl_module_state_t hdl_adc(void *desc, uint8_t enable);
hdl_adc_status_e hdl_adc_start(hdl_adc_t *hdl_adc);
hdl_adc_status_e hdl_adc_get_data(hdl_adc_t *hdl_adc, hdl_adc_channel_e channel, uint16_t *output_data);
hdl_adc_status_e hdl_adc_status(hdl_adc_t *hdl_adc);


#endif