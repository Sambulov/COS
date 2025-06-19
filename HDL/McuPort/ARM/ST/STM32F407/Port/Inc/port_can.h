#ifndef PORT_CAN_H_
#define PORT_CAN_H_

#include "hdl_can.h"

#define HDL_CAN_VAR_SIZE     56

typedef struct {
  uint32_t phy;                   /* CAN1, CAN2 */
  uint32_t rcu;                   /*  */
  hdl_interrupt_t * const interrupts[4]; /* tx, rx0, rx1, sce */
  uint32_t speed;
} hdl_can_mcu_config_t;

/* depends on:
  gpio can_rx
  gpio can_tx
  clock 
  interrupt controller (nvic)
  hdl_timer_t
 */
hdl_module_new_t(hdl_can_mcu_t, HDL_CAN_VAR_SIZE, hdl_can_mcu_config_t*, hdl_can_iface_t);

extern const hdl_can_iface_t hdl_can_mcu_iface;

#endif /* PORT_CAN_H_ */ 
