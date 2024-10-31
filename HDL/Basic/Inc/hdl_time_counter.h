#ifndef HDL_TIME_COUNTER_H_
#define HDL_TIME_COUNTER_H_

#define HDL_TIME_COUNTER_PRV_SIZE         28

typedef struct {
  hdl_interrupt_t *reload_interrupt;
} hdl_time_counter_config_t;

typedef struct {
  hdl_module_t module;
  const hdl_time_counter_config_t *config;
  PRIVATE(hw, HDL_TIME_COUNTER_PRV_SIZE);
} hdl_time_counter_t;

hdl_module_state_t hdl_time_counter(void *desc, const uint8_t enable);
uint32_t hdl_time_counter_get(hdl_time_counter_t *desc);

#endif /* HDL_TIME_COUNTER_H_ */
