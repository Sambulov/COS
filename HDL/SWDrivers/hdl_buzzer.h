#ifndef HDL_BUZZER_H_
#define HDL_BUZZER_H_

#define HDL_BUZZER_VAR_SIZE 36

typedef uint8_t (*hdl_buzzer_set_t)(const void *desc, uint32_t duration, uint8_t volume);

typedef struct {
  hdl_module_initializer_t init;
  hdl_buzzer_set_t set;
} hdl_buzzer_iface_t;

/* depends on
    pwm 
    en_pin - generator mode
    time counter
 */
hdl_module_new_t(hdl_buzzer_t, HDL_BUZZER_VAR_SIZE, void *, hdl_buzzer_iface_t);

extern const hdl_buzzer_iface_t hdl_buzzer_iface;

__STATIC_INLINE uint8_t hdl_buzzer_set(const void *desc, uint32_t duration, uint8_t volume) {
  MODULE_ASSERT(desc, HDL_FALSE);
  return ((hdl_buzzer_iface_t *)((hdl_module_base_t *)desc)->iface)->set(desc, duration, volume);
}

#endif /* HDL_BUZZER_H_ */