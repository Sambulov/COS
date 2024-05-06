#ifndef HDL_ADC_H_
#define HDL_ADC_H_

#include "port_adc.h"

#define HDL_ADC_INVALID_VALUE   0xFFFFFFFF

/* Initialization */
hdl_module_state_t hdl_adc(void *desc, uint8_t enable);
uint32_t hdl_adc_get_data(hdl_adc_t *hdl_adc, hdl_adc_source_t *src);

#endif