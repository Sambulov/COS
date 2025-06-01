#ifndef PORT_EXTI_H_
#define PORT_EXTI_H_

#include "port_exti_spec.h"

typedef enum {
  HDL_EXTI_TRIGGER_FALLING        = 0x01,
  HDL_EXTI_TRIGGER_RISING         = 0x02,
  HDL_EXTI_TRIGGER_RISING_FALLING = HDL_EXTI_TRIGGER_RISING | HDL_EXTI_TRIGGER_FALLING,
  HDL_EXTI_TRIGGER_NONE           = !(HDL_EXTI_TRIGGER_RISING | HDL_EXTI_TRIGGER_FALLING)
} hdl_exti_trigger_t;

typedef enum {
  HDL_EXTI_SOURCE_PA = 0b0000,
  HDL_EXTI_SOURCE_PB = 0b0001,
  HDL_EXTI_SOURCE_PC = 0b0010,
  HDL_EXTI_SOURCE_PD = 0b0011,
  HDL_EXTI_SOURCE_PE = 0b0100,
  HDL_EXTI_SOURCE_PF = 0b0101,
  HDL_EXTI_SOURCE_PG = 0b0110,
} hdl_exti_source_t;

typedef enum {
  HDL_EXTI_MODE_INTERRUPT = EXTI_INTERRUPT,
  HDL_EXTI_MODE_EVENT = EXTI_EVENT
} hdl_exti_mode_t;

typedef struct {
  hdl_exti_line_t line;
  hdl_exti_source_t source;
  hdl_exti_mode_t mode;
  hdl_exti_trigger_t trigger;
} hdl_exti_t;

typedef struct {
  uint32_t phy;
  const hdl_exti_t * const *extis;
} hdl_exti_controller_config_t;

#define hdl_extis(...) ((const hdl_exti_t * const []){__VA_ARGS__, NULL})

typedef void (*hdl_exti_sw_trigger_t)(const void *, hdl_exti_line_t);

typedef struct {
  hdl_module_initializer_t init;
  hdl_exti_sw_trigger_t trigger;
} hdl_exti_controller_iface_t;

/* 
  depends on:
  nvic
*/
hdl_module_new_t(hdl_exti_controller_t, 0, hdl_exti_controller_config_t, hdl_exti_controller_iface_t);

extern const hdl_exti_controller_iface_t hdl_exti_controller_iface;

__STATIC_INLINE void hdl_exti_sw_trigger(const void *exti, hdl_exti_line_t line) {
  ((hdl_exti_controller_iface_t *)((hdl_module_base_t *)exti)->iface)->trigger(exti, line);
}

#endif // PORT_EXTI_H_