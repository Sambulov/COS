
#include "hdl.h"
#include "CodeLib.h"

typedef struct {
  coroutine_t worker;
  hdl_i2c_message_t i2c_msg;
  uint8_t state;

  uint8_t read_reg_addr;
  uint8_t input_port0;
  uint8_t input_port1;
  uint8_t output_port0;
  uint8_t output_port1;
  uint8_t polarity_port0;
  uint8_t polarity_port1;
  uint8_t config_port0;
  uint8_t config_port1;

  uint8_t write_reg_addr;
  uint8_t user_output_port0;
  uint8_t user_output_port1;
  uint8_t user_polarity_port0;
  uint8_t user_polarity_port1;
  uint8_t user_config_port0;
  uint8_t user_config_port1;
} hdl_xl9535_port_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_xl9535_port_var_t, *((hdl_xl9535_port_t *)0)->obj_var, HDL_XL9535_PORT_VAR_SIZE, xl9535.h);

#define XL9535_STATE_WORK               0x07
#define XL9535_STATE_IDLE               0x00

#define XL9535_STATE_READ_SEL           0x01
#define XL9535_STATE_READ_RECEIVE       0x02
#define XL9535_STATE_READ_COMPLETE      0x03

#define XL9535_STATE_WRITE_SEL          0x04
#define XL9535_STATE_WRITE_DATA         0x05
#define XL9535_STATE_WRITE_COMPLETE     0x06

#define XL9535_STATE_FLAGS              0xf8
#define XL9535_STATE_SEND               0x08
#define XL9535_STATE_AWAITE             0x10
#define XL9535_STATE_RESET              0x20
#define XL9535_STATE_SYNC_R             0x40
#define XL9535_STATE_SYNC_W             0x80

static uint8_t _xl9535_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_xl9535_port_t *port = (hdl_xl9535_port_t *)arg;
  hdl_xl9535_port_var_t *port_var = (hdl_xl9535_port_var_t *)port->obj_var;
  hdl_i2c_t *i2c = (hdl_i2c_t *)port->dependencies[0];
  if(port_var->state & XL9535_STATE_RESET) {
    port_var->i2c_msg.length = 0;
    port_var->i2c_msg.options = HDL_I2C_MESSAGE_STOP;
    port_var->state |= XL9535_STATE_SEND;
    port_var->state &= ~XL9535_STATE_RESET;
    if(port_var->state & XL9535_STATE_SYNC_W) {
      port_var->write_reg_addr = 2;
      HDL_REG_MODIFY(port_var->state, XL9535_STATE_WORK, XL9535_STATE_WRITE_SEL);
    }
    else if (port_var->state & XL9535_STATE_SYNC_R) {
      port_var->read_reg_addr = 0;
      HDL_REG_MODIFY(port_var->state, XL9535_STATE_WORK, XL9535_STATE_READ_SEL);
    }
  }
  if(port_var->state & XL9535_STATE_SEND) {
    if(hdl_i2c_transfer(i2c, &port_var->i2c_msg)) {
      port_var->state &= ~XL9535_STATE_SEND;
      port_var->state |= XL9535_STATE_AWAITE;
    }
  }
  else if(port_var->state & XL9535_STATE_AWAITE) {
    if(port_var->i2c_msg.status & HDL_I2C_MESSAGE_STATUS_COMPLETE) {
      port_var->state &= ~XL9535_STATE_AWAITE;
    }
  }
  else switch (port_var->state & XL9535_STATE_WORK) {
    case XL9535_STATE_READ_SEL: {
      port_var->i2c_msg.buffer = &port_var->read_reg_addr;
      port_var->i2c_msg.length = 1;
      port_var->i2c_msg.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR;
      port_var->state |= XL9535_STATE_SEND;
      HDL_REG_MODIFY(port_var->state, XL9535_STATE_WORK, XL9535_STATE_READ_RECEIVE);
      break;
    }

    case XL9535_STATE_READ_RECEIVE: {
      if(port_var->i2c_msg.status & HDL_I2C_MESSAGE_FAULT_MASK) port_var->state |= XL9535_STATE_RESET;
      else {
        port_var->i2c_msg.buffer = (&port_var->input_port0) + port_var->read_reg_addr;
        port_var->i2c_msg.length = 2;
        port_var->i2c_msg.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_MRSW | HDL_I2C_MESSAGE_NACK_LAST | HDL_I2C_MESSAGE_STOP;
        port_var->state |= XL9535_STATE_SEND;
        HDL_REG_MODIFY(port_var->state, XL9535_STATE_WORK, XL9535_STATE_READ_COMPLETE);
      }
      break;
    }

    case XL9535_STATE_READ_COMPLETE: {
      if(port_var->i2c_msg.status & HDL_I2C_MESSAGE_FAULT_MASK) port_var->state |= XL9535_STATE_RESET;
      else {
        port_var->read_reg_addr += 2;
        if(port_var->read_reg_addr >= 8) {
          HDL_REG_MODIFY(port_var->state, XL9535_STATE_WORK, XL9535_STATE_IDLE);
          port_var->read_reg_addr = 0;
        }
        else
          HDL_REG_MODIFY(port_var->state, XL9535_STATE_WORK, XL9535_STATE_READ_SEL);
      }
      break;
    }

    case XL9535_STATE_WRITE_SEL: {
      port_var->i2c_msg.buffer = &port_var->write_reg_addr;
      port_var->i2c_msg.length = 1;
      port_var->i2c_msg.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR; 
      port_var->state |= XL9535_STATE_SEND;
      HDL_REG_MODIFY(port_var->state, XL9535_STATE_WORK, XL9535_STATE_WRITE_DATA);
      break;
    }

    case XL9535_STATE_WRITE_DATA: {
      if(port_var->i2c_msg.status & HDL_I2C_MESSAGE_FAULT_MASK) port_var->state |= XL9535_STATE_RESET;
      else {
        port_var->i2c_msg.buffer = (&port_var->user_output_port0) + (port_var->write_reg_addr - 2);
        port_var->i2c_msg.length = 2;
        port_var->i2c_msg.options = HDL_I2C_MESSAGE_STOP;
        port_var->state |= XL9535_STATE_SEND;
        HDL_REG_MODIFY(port_var->state, XL9535_STATE_WORK, XL9535_STATE_WRITE_COMPLETE);
      }
      break;
    }

    case XL9535_STATE_WRITE_COMPLETE: {
      if(port_var->i2c_msg.status & HDL_I2C_MESSAGE_FAULT_MASK) port_var->state |= XL9535_STATE_RESET;
      else {
        port_var->write_reg_addr += 2;
        if(port_var->write_reg_addr >= 8) {
          HDL_REG_SET(port_var->state, XL9535_STATE_SYNC_R);
          HDL_REG_MODIFY(port_var->state, XL9535_STATE_WORK, XL9535_STATE_IDLE);
          port_var->write_reg_addr = 2;
        }
        else {
          HDL_REG_MODIFY(port_var->state, XL9535_STATE_WORK, XL9535_STATE_WRITE_SEL);
        }
      }
      break;
    }

    case XL9535_STATE_IDLE:
    default: {
      if(port_var->state & XL9535_STATE_SYNC_R) {
        HDL_REG_MODIFY(port_var->state, XL9535_STATE_WORK, XL9535_STATE_READ_SEL);
        HDL_REG_CLEAR(port_var->state, XL9535_STATE_SYNC_R);
      }
      else if(port_var->state & XL9535_STATE_SYNC_W) {
        HDL_REG_MODIFY(port_var->state, XL9535_STATE_WORK, XL9535_STATE_WRITE_SEL);
        HDL_REG_CLEAR(port_var->state, XL9535_STATE_SYNC_W);
      }
      //todo read int
      break;
    }
  }
  return cancel;
}

static hdl_module_state_t _hdl_xl9535_port(const void *desc, const uint8_t enable) {
  hdl_xl9535_port_t *port = (hdl_xl9535_port_t *)desc;
  hdl_xl9535_port_var_t *port_var = (hdl_xl9535_port_var_t *)port->obj_var;
  if(enable) {
    port_var->state = XL9535_STATE_SYNC_R | XL9535_STATE_IDLE;
    port_var->input_port0 = 0x00;
    port_var->input_port1 = 0x00;
    port_var->output_port0 = port_var->user_output_port0 = 0xff;
    port_var->output_port1 = port_var->user_output_port1 = 0xff;
    port_var->polarity_port0 = port_var->user_polarity_port0 = 0x00;
    port_var->polarity_port1 = port_var->user_polarity_port1 = 0x00;
    port_var->config_port0 = port_var->user_config_port0 = 0xff;
    port_var->config_port1 = port_var->user_config_port1 = 0xff;

    port_var->read_reg_addr = 0;
    port_var->write_reg_addr = 2;

    port_var->i2c_msg.status = 0;
    port_var->i2c_msg.buffer = &port_var->input_port0;
    port_var->i2c_msg.address = port->config->address;

    coroutine_add(&port_var->worker, &_xl9535_worker, port);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&port_var->worker);
  return HDL_MODULE_UNLOADED;
}

static hdl_module_state_t _hdl_xl9535_pin(const void *desc, const uint8_t enable) {
  hdl_xl9535_pin_t *pin = (hdl_xl9535_pin_t *)desc;
  if(enable) {
    hdl_xl9535_port_t *port = (hdl_xl9535_port_t *)pin->dependencies[0];
    hdl_xl9535_port_var_t *port_var = (hdl_xl9535_port_var_t *)port->obj_var;
    uint32_t pin_no = (uint32_t)pin->config->pin_no;
    uint8_t *mode = &port_var->user_config_port0;
    uint8_t *output = &port_var->user_output_port0;
    uint8_t *pol = &port_var->user_polarity_port0;
    if(pin_no >= 8) {
      mode = &port_var->user_config_port1;
      output = &port_var->user_output_port1;
      pol = &port_var->user_polarity_port1;
      pin_no -= 8;
    }
    uint8_t mask = 1 << pin_no;
    HDL_REG_MODIFY(*mode, mask, (pin->config->mode << pin_no));
    HDL_REG_MODIFY(*output, mask, (pin->config->output << pin_no));
    HDL_REG_MODIFY(*pol, mask, (pin->config->polarity << pin_no));
    port_var->state |= XL9535_STATE_SYNC_W;
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_UNLOADED;
}

static hdl_gpio_state _hdl_xl9535_read_io(const void *desc, uint8_t io) {
  if (hdl_state(desc) == HDL_MODULE_FAULT) return HDL_GPIO_LOW;
  hdl_xl9535_pin_t *pin = (hdl_xl9535_pin_t *)desc;
  hdl_xl9535_port_t *port = (hdl_xl9535_port_t *)pin->dependencies[0];
  hdl_xl9535_port_var_t *port_var = (hdl_xl9535_port_var_t *)port->obj_var;
  uint32_t pin_no = (uint32_t)pin->config->pin_no;
  uint8_t *reg = io? &port_var->input_port0: &port_var->output_port0;
  if(pin_no >= 8) {
    reg = io? &port_var->input_port1: &port_var->output_port1;
    pin_no -= 8;
  }
  uint8_t mask = 1 << pin_no;
  return (*reg & mask)? HDL_GPIO_HIGH: HDL_GPIO_LOW;
}


static hdl_gpio_state _hdl_xl9535_read(const void *desc) {
  return _hdl_xl9535_read_io(desc, 1);
}

static hdl_gpio_state _hdl_xl9535_read_output(const void *desc) {
  return _hdl_xl9535_read_io(desc, 0);
}

static void _hdl_xl9535_write_io(const void *desc, const hdl_gpio_state state, uint8_t toggle) {
  if (hdl_state(desc) == HDL_MODULE_FAULT) return;
  hdl_xl9535_pin_t *pin = (hdl_xl9535_pin_t *)desc;
  hdl_xl9535_port_t *port = (hdl_xl9535_port_t *)pin->dependencies[0];
  hdl_xl9535_port_var_t *port_var = (hdl_xl9535_port_var_t *)port->obj_var;
  uint32_t pin_no = (uint32_t)pin->config->pin_no;
  uint8_t *reg = &port_var->user_output_port0;
  if(pin_no >= 8) {
    reg = &port_var->user_output_port1;
    pin_no -= 8;
  }
  uint8_t mask = 1 << pin_no;
  uint8_t val;
  if(toggle) val = *reg ^ mask;
  else val = (state == HDL_GPIO_HIGH)? mask: 0;
  if(*reg ^ val) port_var->state |= XL9535_STATE_SYNC_W;
  HDL_REG_MODIFY(*reg, mask, val);
}

static void _hdl_xl9535_write(const void *desc, const hdl_gpio_state state) {
  _hdl_xl9535_write_io(desc, state, 0);
}

static void _hdl_xl9535_toggle(const void *desc) {
  _hdl_xl9535_write_io(desc, 0, 1);
}

const hdl_module_base_iface_t hdl_xl9535_port_iface = {
  .init = &_hdl_xl9535_port
};

const hdl_gpio_pin_iface_t hdl_xl9535_pin_iface = {
  .init = &_hdl_xl9535_pin,
  .read = &_hdl_xl9535_read,
  .read_ouput = &_hdl_xl9535_read_output,
  .write = &_hdl_xl9535_write,
  .toggle = &_hdl_xl9535_toggle
};
