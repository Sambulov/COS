#ifndef HDL_ADC_H_
#define HDL_ADC_H_

#define HDL_ADC_INVALID_VALUE   0xFFFFFFFF

typedef uint32_t (*hdl_adc_get_t)(const void *desc, uint32_t src_id);
typedef uint32_t (*hdl_adc_age_t)(const void *desc);

typedef struct {
  hdl_module_initializer_t init;
  hdl_adc_get_t get;
  hdl_adc_age_t age;
} hdl_adc_iface_t;

hdl_module_state_t hdl_adc(void *desc, uint8_t enable);

__STATIC_INLINE uint32_t hdl_adc_get(const void *desc, uint32_t src_id) {
  return ((hdl_adc_iface_t *)((hdl_module_base_t *)desc)->iface)->get(desc, src_id);
}

__STATIC_INLINE uint32_t hdl_adc_age(const void *desc) {
  return ((hdl_adc_iface_t *)((hdl_module_base_t *)desc)->iface)->age(desc);
}

#endif /* HDL_ADC_H_ */