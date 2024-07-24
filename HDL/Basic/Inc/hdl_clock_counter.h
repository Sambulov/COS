#ifndef HDL_CLOCK_COUNTER_H_
#define HDL_CLOCK_COUNTER_H_

typedef enum {
  HDL_UP_COUNTER,
  HDL_DOWN_COUNTER,
} hdl_clock_counter_dir_t;

typedef struct {
  hdl_module_t module;
  uint32_t counter_reload;
  hdl_clock_counter_dir_t diction;
} hdl_clock_counter_t;

hdl_module_state_t hdl_clock_counter(void *desc, const uint8_t enable);
uint32_t hdl_clock_counter_get(hdl_clock_counter_t *desc);

#endif /* HDL_CLOCK_COUNTER_H_ */ 
