#ifndef HDL_ADC_H_
#define HDL_ADC_H_

#define HDL_ADC_INVALID_VALUE   0xFFFFFFFF

typedef uint32_t (*hdl_adc_ch_get_t)(const void *desc);

typedef struct {
  hdl_module_initializer_t init;
  hdl_adc_ch_get_t value;
  hdl_adc_ch_get_t age;
} hdl_adc_ch_iface_t;

hdl_module_new_t(hdl_adc_ch_t, 0, void*, hdl_adc_ch_iface_t);

__STATIC_INLINE uint32_t hdl_adc_ch_get(const void *desc) {
  MODULE_ASSERT(desc, 0xFFFFFFFF);
  return ((hdl_adc_ch_iface_t *)((hdl_module_base_t *)desc)->iface)->value(desc);
}

__STATIC_INLINE uint32_t hdl_adc_ch_age(const void *desc) {
  MODULE_ASSERT(desc, 0);
  return ((hdl_adc_ch_iface_t *)((hdl_module_base_t *)desc)->iface)->age(desc);
}

#endif /* HDL_ADC_H_ */