#ifndef HDL_ADC_H_
#define HDL_ADC_H_

#include "port_adc.h"

#define HDL_ADC_INVALID_VALUE   0xFFFFFFFF

/* 
*   TODO: oversampling don`t support, discontinuos operation mode don`t support, single operation don`t support,
*   conversion result threshold monitor function don`t support 
*/
// typedef enum {
//     HDL_ADC_STATUS_INIT_FAILED = 0,
//     HDL_ADC_STATUS_WAITING_START_TRIGGER,
//     HDL_ADC_STATUS_DATA_READY,
//     HDL_ADC_STATUS_ONGOING,
// } hdl_adc_status_e;

/* Initialization */
hdl_module_state_t hdl_adc(void *desc, uint8_t enable);
uint32_t hdl_adc_get_data(hdl_adc_t *hdl_adc, hdl_adc_source_t *src);
//hdl_adc_status_e hdl_adc_start(hdl_adc_t *hdl_adc);
//hdl_adc_status_e hdl_adc_status(hdl_adc_t *hdl_adc);


#endif