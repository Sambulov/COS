#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_spi_message_t msg;
  uint8_t state;
} hdl_74hc595_port_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_74hc595_port_var_t, *((hdl_74hc595_port_t *)0)->obj_var, HDL_74HC595_PORT_VAR_SIZE, "74hc595.h");

#define _74HC595_STATE_SYNC            0x80
#define _74HC595_STATE_PUSH_MSG        0x40
#define _74HC595_STATE_TRANSFER        0x20

#define state_regs(port)  (((uint8_t *)port->config->port_regs))
#define set_regs(port)  (&((uint8_t *)port->config->port_regs)[(port->config->shift_reg_length >> 3)])

#define pin_to_reg_index(pin_no)   (pin_no >> 3)

static uint8_t _74hc595_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_74hc595_port_t *port = (hdl_74hc595_port_t *)arg;
  hdl_spi_client_ch_t *spi = (hdl_spi_client_ch_t *)port->dependencies[0];
  hdl_74hc595_port_var_t *port_var = (hdl_74hc595_port_var_t *)port->obj_var;
  hdl_gpio_pin_t *latch = (hdl_gpio_pin_t *)port->dependencies[1];
  hdl_gpio_pin_t *oe = (hdl_gpio_pin_t *)port->dependencies[3];
  if(port_var->state & _74HC595_STATE_TRANSFER) {
    if(port_var->msg.status & HDL_SPI_MESSAGE_STATUS_COMPLETE) {
      hdl_gpio_set_active(oe);
      hdl_gpio_set_active(latch);
      port_var->state &= ~_74HC595_STATE_TRANSFER;
    }
  }
  if(!(port_var->state & _74HC595_STATE_TRANSFER)) {
    if((port_var->state & _74HC595_STATE_PUSH_MSG) && hdl_spi_client_ch_transfer(spi, &port_var->msg)) {
      port_var->state |= _74HC595_STATE_TRANSFER;
      port_var->state &= ~_74HC595_STATE_PUSH_MSG;
    }
    else if(port_var->state & _74HC595_STATE_SYNC) {
      hdl_gpio_set_inactive(latch);
      port_var->msg.options = HDL_SPI_MESSAGE_CH_SELECT | HDL_SPI_MESSAGE_CH_RELEASE;
      port_var->msg.rx_buffer = NULL;
      port_var->msg.tx_buffer = state_regs(port),
      port_var->msg.tx_len = pin_to_reg_index(port->config->shift_reg_length);
      mem_cpy(state_regs(port), set_regs(port), port_var->msg.tx_len);
      port_var->state |= _74HC595_STATE_PUSH_MSG;
      port_var->state &= ~_74HC595_STATE_SYNC;
    }
  }
  return cancel;
}

static hdl_module_state_t _hdl_74hc595_port(const void *desc, const uint8_t enable) {
  hdl_74hc595_port_t *port = (hdl_74hc595_port_t *)desc;
  hdl_74hc595_port_var_t *port_var = (hdl_74hc595_port_var_t *)port->obj_var;
  hdl_gpio_pin_t *oe = (hdl_gpio_pin_t *)port->dependencies[3];
  hdl_gpio_pin_t *mr = (hdl_gpio_pin_t *)port->dependencies[2];
  hdl_gpio_set_inactive(oe);
  hdl_gpio_set_active(mr);
  if(enable) {
    uint8_t *setr = set_regs(port);
    uint8_t *star = state_regs(port);
    for (uint32_t i = 0; i < pin_to_reg_index(port->config->shift_reg_length); i++) {
      setr[i] = 0;
      star[i] = 0;
    }
    port_var->state = _74HC595_STATE_SYNC;
    coroutine_add(&port_var->worker, &_74hc595_worker, port);
    hdl_gpio_set_inactive(mr);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&port_var->worker);
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_74hc595_pin(const void *desc, const uint8_t enable) {
  hdl_74hc595_pin_t *pin = (hdl_74hc595_pin_t *)desc;
  if(enable) {
    hdl_74hc595_port_t *port = (hdl_74hc595_port_t *)pin->dependencies[0];
    hdl_74hc595_port_var_t *port_var = (hdl_74hc595_port_var_t *)port->obj_var;
    uint32_t pin_no = pin->config->pin;
    if(pin_no > port->config->shift_reg_length) return HDL_MODULE_FAULT;    
    uint8_t *reg = &set_regs(port)[pin_to_reg_index(pin_no)];
    uint8_t mask = (1 << (pin_no & 0x07));
    CL_REG_MODIFY(*reg, mask, ((pin->config->inactive_default == HDL_GPIO_HIGH)? mask: 0));
    CL_REG_SET(port_var->state, _74HC595_STATE_SYNC);
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

static hdl_gpio_state _hdl_74hc595_read_output(const void *desc) {
  hdl_74hc595_pin_t *pin = (hdl_74hc595_pin_t *)desc;
  hdl_74hc595_port_t *port = (hdl_74hc595_port_t *)pin->dependencies[0];
  uint32_t pin_no = pin->config->pin;
  if(pin_no > port->config->shift_reg_length) return HDL_GPIO_LOW;
  uint8_t *reg = &state_regs(port)[pin_to_reg_index(pin_no)];
  uint8_t mask = (1 << (pin_no & 0x07));
  return (*reg & mask)? HDL_GPIO_HIGH: HDL_GPIO_LOW;
}

static void _hdl_74hc595_write_io(const void *desc, const hdl_gpio_state state, uint8_t toggle) {
  hdl_74hc595_pin_t *pin = (hdl_74hc595_pin_t *)desc;
  hdl_74hc595_port_t *port = (hdl_74hc595_port_t *)pin->dependencies[0];
  hdl_74hc595_port_var_t *port_var = (hdl_74hc595_port_var_t *)port->obj_var;
  uint32_t pin_no = pin->config->pin;
  if(pin_no > port->config->shift_reg_length) return;
  uint8_t mask = (1 << (pin_no & 0x07));
  uint8_t *reg = &set_regs(port)[pin_to_reg_index(pin_no)];
  if(toggle) *reg ^= mask;
  else CL_REG_MODIFY(*reg, mask, ((state == HDL_GPIO_HIGH)? mask: 0));
  CL_REG_SET(port_var->state, _74HC595_STATE_SYNC);
}

static void _hdl_74hc595_write(const void *desc, const hdl_gpio_state state) {
  _hdl_74hc595_write_io(desc, state, 0);
}

static void _hdl_74hc595_toggle(const void *desc) {
  _hdl_74hc595_write_io(desc, 0, 1);
}

const hdl_module_base_iface_t hdl_74hc595_port_iface = {
  .init = &_hdl_74hc595_port
};

const hdl_gpio_pin_iface_t hdl_74hc595_pin_iface = {
  .init = &_hdl_74hc595_pin,
  .read = &_hdl_74hc595_read_output,
  .read_ouput = &_hdl_74hc595_read_output,
  .write = &_hdl_74hc595_write,
  .toggle = &_hdl_74hc595_toggle
};
