#ifndef ATB3500_RS485_CTRL_H_
#define ATB3500_RS485_CTRL_H_

#define ATB3500_RS485_CTRL_MAGIC          ((uint32_t)(0xBADFEED0))
#define ATB3500_RS485_CTRL_PRV_SIZE       64

#define ATB3500_RS485_CTRL_OVN            ((uint32_t)(0xDE47F35C))

typedef struct {
	uint32_t boud;
	uint32_t xfer_unit_size;
	uint32_t magic;
} atb3500_rs485_ctrl_proto_rx_t;

typedef struct {
	uint32_t boud;
	uint32_t xfer_unit_size;
	uint32_t ovn;
} atb3500_rs485_ctrl_proto_tx_t;

typedef struct {
  hdl_interrupt_t *counter_reload_int;
  hdl_interrupt_t *tx_pin_int;
  uint32_t counter_trigger;
  uint32_t pin_trigger;
  uint32_t boud_rate_default;
  uint32_t trunsfer_unit_size;
} atb3500_rs485_ctrl_config_t;

/*
  depends on:
  hdl_tick_counter_t
  hdl_interrupt_controller_t
  hdl_gpio_pin_t < tx_pin
  hdl_gpio_pin_t < driver_pin

*/
typedef struct {
    hdl_module_t module;
    const atb3500_rs485_ctrl_config_t *config;
    PRIVATE(bldl, ATB3500_RS485_CTRL_PRV_SIZE);
} atb3500_rs485_ctrl_t;

hdl_module_state_t atb3500_rs485_ctrl(void *desc, uint8_t enable);
atb3500_rs485_ctrl_proto_tx_t *atb3500_rs485_ctrl_update(atb3500_rs485_ctrl_t *desc, atb3500_rs485_ctrl_proto_rx_t *rx_data);

#endif //ATB3500_RS485_CTRL_H_
