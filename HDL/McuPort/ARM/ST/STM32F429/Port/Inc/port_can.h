#ifndef PORT_CAN_H_
#define PORT_CAN_H_

#include "hdl_can.h"

#define HDL_CAN_VAR_SIZE     72

/*
  depends on:
  clock

  Config CAN2 fifler bank number 0~28
  0 : No one filter for CAN1
  28: No one filter for CAN2
  14: (default) 14 for CAN1 & 14 for CAN2
*/
hdl_module_new_t(hdl_can_mcu_base_t, 0, uint32_t, hdl_module_base_iface_t);

extern const hdl_module_base_iface_t hdl_can_mcu_base_iface;

typedef struct {
  uint32_t phy;                   /* CAN1, CAN2 */
  uint32_t rcc;                   /* RCC_APB1ENR_CAN1EN, RCC_APB1ENR_CAN2EN */
  /* Bit time
              seg1     seg2
         |<--------->|<---->|
    |Sync|Prop|Phase1|Phase2|
    |<-->|           ^
      1QT       Semple point
    Sample point must be within 50 ~ 90% of the bit time. 87.5% is the preffered by CANOpen & DeviceNet, 75% by ARINC 825
    ((sync + seg1) : seg2 ~ 87.5 : 12.5)
  */
  uint32_t prescaler;         /* for TQ (time quantum) */
  uint32_t time_seg1;         /* CAN_BS1_[1..16]TQ */
  uint32_t time_seg2;         /* CAN_BS2_[1..8]TQ */
  uint32_t sync_jump_width;   /* CAN_SJW_[1, 2, 3, 4]TQ ReSynchronization Jamp Width */
  uint32_t mode;              /* CAN_MODE_NORMAL, CAN_MODE_LOOPBACK, CAN_MODE_SILENT, CAN_MODE_SILENT_LOOPBACK */
  uint8_t time_triggered_mode    : 1, /* Time Triggered Communication Mode, time master */
          auto_bus_off           : 1, /* Enable bus fault auto recovery */
          auto_wake_up           : 1, /* System wakeup capability */
          auto_retransmission    : 1, /* autoretransmit in case of NACK, must be disabled if time_triggered_mode=1 */
          receive_fifo_locked    : 1, /* 0: frame rewrite on fifo overflow, 1: loose new frames */
          transmit_fifo_priority : 1; /* 0: TX messages priority mode, 1: TX messages fifo mode */
  const hdl_can_filter_t * const *filters;
  //hdl_interrupt_t *tx_int;
  //hdl_interrupt_t *rx0_int;
  //hdl_interrupt_t *rx1_int;
  //hdl_interrupt_t *sce_int;
} hdl_can_mcu_config_t;

/* depends on:
  hdl_can_mcu_base_t
  gpio can_rx
  gpio can_tx
 */
hdl_module_new_t(hdl_can_mcu_t, HDL_CAN_VAR_SIZE, hdl_can_mcu_config_t*, hdl_can_iface_t);

extern const hdl_can_iface_t hdl_can_mcu_iface;

#endif /* PORT_CAN_H_ */ 
