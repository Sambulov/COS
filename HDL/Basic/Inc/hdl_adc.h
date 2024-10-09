#ifndef HDL_ADC_H_
#define HDL_ADC_H_

#include "port_adc.h"

#define HDL_ADC_INVALID_VALUE   0xFFFFFFFF

typedef struct{
    hdl_module_t module;
    const hdl_adc_config_t *config;
    PRIVATE(hw, HDL_ADC_PRV_SIZE);
} hdl_adc_t;

hdl_module_state_t hdl_adc(void *desc, uint8_t enable);

uint32_t hdl_adc_get(hdl_adc_t *hdl_adc, uint32_t src);
uint32_t hdl_adc_age(hdl_adc_t *hdl_adc);

#endif /* HDL_ADC_H_ */