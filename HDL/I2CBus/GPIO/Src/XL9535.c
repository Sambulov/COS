
#include "hdl.h"
#include "CodeLib.h"

// typedef struct {
//   hdl_module_t module;
//   const hdl_xl9535_port_config_t *config;
//   struct {
//     coroutine_t worker;
//     hdl_i2c_message_t i2c_msg;
//     uint8_t state;

//     uint8_t read_reg_addr;
//     uint8_t input_port0;
//     uint8_t input_port1;
//     uint8_t output_port0;
//     uint8_t output_port1;
//     uint8_t polarity_port0;
//     uint8_t polarity_port1;
//     uint8_t config_port0;
//     uint8_t config_port1;

//     uint8_t write_reg_addr;
//     uint8_t user_output_port0;
//     uint8_t user_output_port1;
//     uint8_t user_polarity_port0;
//     uint8_t user_polarity_port1;
//     uint8_t user_config_port0;
//     uint8_t user_config_port1;
//   } private;
// } hdl_xl9535_port_private_t;

// #define hdl_gpio_pin_config(...) (const hdl_gpio_pin_config_t []){{__VA_ARGS__}}

// HDL_ASSERRT_STRUCTURE_CAST(hdl_xl9535_port_private_t, hdl_xl9535_port_t, HDL_XL9535_PORT_PRV_SIZE, xl9535.h);

// #define XL9535_STATE_WORK               0x07
// #define XL9535_STATE_IDLE               0x00

// #define XL9535_STATE_READ_SEL           0x01
// #define XL9535_STATE_READ_RECEIVE       0x02
// #define XL9535_STATE_READ_COMPLETE      0x03

// #define XL9535_STATE_WRITE_SEL          0x04
// #define XL9535_STATE_WRITE_DATA         0x05
// #define XL9535_STATE_WRITE_COMPLETE     0x06

// #define XL9535_STATE_FLAGS              0xf8
// #define XL9535_STATE_SEND               0x08
// #define XL9535_STATE_AWAITE             0x10
// #define XL9535_STATE_RESET              0x20
// #define XL9535_STATE_SYNC_R             0x40
// #define XL9535_STATE_SYNC_W             0x80

// static uint8_t _xl9535_worker(coroutine_t *this, uint8_t cancel, void *arg) {
//   hdl_xl9535_port_private_t *gpio = (hdl_xl9535_port_private_t *)arg;
//   hdl_i2c_t *i2c = (hdl_i2c_t *)gpio->module.dependencies[0];

//   if(gpio->private.state & XL9535_STATE_RESET) {
//     gpio->private.i2c_msg.length = 0;
//     gpio->private.i2c_msg.options = HDL_I2C_MESSAGE_STOP;
//     gpio->private.state |= XL9535_STATE_SEND;
//     gpio->private.state &= ~XL9535_STATE_RESET;
//     if(gpio->private.state & XL9535_STATE_SYNC_W) {
//       gpio->private.write_reg_addr = 2;
//       HDL_REG_MODIFY(gpio->private.state, XL9535_STATE_WORK, XL9535_STATE_WRITE_SEL);
//     }
//     else if (gpio->private.state & XL9535_STATE_SYNC_R) {
//       gpio->private.read_reg_addr = 0;
//       HDL_REG_MODIFY(gpio->private.state, XL9535_STATE_WORK, XL9535_STATE_READ_SEL);
//     }
//   }
//   if(gpio->private.state & XL9535_STATE_SEND) {
//     if(hdl_i2c_transfer_message(i2c, &gpio->private.i2c_msg)) {
//       gpio->private.state &= ~XL9535_STATE_SEND;
//       gpio->private.state |= XL9535_STATE_AWAITE;
//     }
//   }
//   else if(gpio->private.state & XL9535_STATE_AWAITE) {
//     if(gpio->private.i2c_msg.status & HDL_I2C_MESSAGE_STATUS_COMPLETE) {
//       gpio->private.state &= ~XL9535_STATE_AWAITE;
//     }
//   }
//   else switch (gpio->private.state & XL9535_STATE_WORK) {
//     case XL9535_STATE_READ_SEL: {
//       gpio->private.i2c_msg.buffer = &gpio->private.read_reg_addr;
//       gpio->private.i2c_msg.length = 1;
//       gpio->private.i2c_msg.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR;
//       gpio->private.state |= XL9535_STATE_SEND;
//       HDL_REG_MODIFY(gpio->private.state, XL9535_STATE_WORK, XL9535_STATE_READ_RECEIVE);
//       break;
//     }

//     case XL9535_STATE_READ_RECEIVE: {
//       if(gpio->private.i2c_msg.status & HDL_I2C_MESSAGE_FAULT_MASK) gpio->private.state |= XL9535_STATE_RESET;
//       else {
//         gpio->private.i2c_msg.buffer = (&gpio->private.input_port0) + gpio->private.read_reg_addr;
//         gpio->private.i2c_msg.length = 2;
//         gpio->private.i2c_msg.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR | HDL_I2C_MESSAGE_MRSW | HDL_I2C_MESSAGE_NACK_LAST | HDL_I2C_MESSAGE_STOP;
//         gpio->private.state |= XL9535_STATE_SEND;
//         HDL_REG_MODIFY(gpio->private.state, XL9535_STATE_WORK, XL9535_STATE_READ_COMPLETE);
//       }
//       break;
//     }

//     case XL9535_STATE_READ_COMPLETE: {
//       if(gpio->private.i2c_msg.status & HDL_I2C_MESSAGE_FAULT_MASK) gpio->private.state |= XL9535_STATE_RESET;
//       else {
//         gpio->private.read_reg_addr += 2;
//         if(gpio->private.read_reg_addr >= 8) {
//           HDL_REG_MODIFY(gpio->private.state, XL9535_STATE_WORK, XL9535_STATE_IDLE);
//           gpio->private.read_reg_addr = 0;
//         }
//         else
//           HDL_REG_MODIFY(gpio->private.state, XL9535_STATE_WORK, XL9535_STATE_READ_SEL);
//       }
//       break;
//     }

//     case XL9535_STATE_WRITE_SEL: {
//       gpio->private.i2c_msg.buffer = &gpio->private.write_reg_addr;
//       gpio->private.i2c_msg.length = 1;
//       gpio->private.i2c_msg.options = HDL_I2C_MESSAGE_START | HDL_I2C_MESSAGE_ADDR; 
//       gpio->private.state |= XL9535_STATE_SEND;
//       HDL_REG_MODIFY(gpio->private.state, XL9535_STATE_WORK, XL9535_STATE_WRITE_DATA);
//       break;
//     }

//     case XL9535_STATE_WRITE_DATA: {
//       if(gpio->private.i2c_msg.status & HDL_I2C_MESSAGE_FAULT_MASK) gpio->private.state |= XL9535_STATE_RESET;
//       else {
//         gpio->private.i2c_msg.buffer = (&gpio->private.user_output_port0) + (gpio->private.write_reg_addr - 2);
//         gpio->private.i2c_msg.length = 2;
//         gpio->private.i2c_msg.options = HDL_I2C_MESSAGE_STOP;
//         gpio->private.state |= XL9535_STATE_SEND;
//         HDL_REG_MODIFY(gpio->private.state, XL9535_STATE_WORK, XL9535_STATE_WRITE_COMPLETE);
//       }
//       break;
//     }

//     case XL9535_STATE_WRITE_COMPLETE: {
//       if(gpio->private.i2c_msg.status & HDL_I2C_MESSAGE_FAULT_MASK) gpio->private.state |= XL9535_STATE_RESET;
//       else {
//         gpio->private.write_reg_addr += 2;
//         if(gpio->private.write_reg_addr >= 8) {
//           HDL_REG_SET(gpio->private.state, XL9535_STATE_SYNC_R);
//           HDL_REG_MODIFY(gpio->private.state, XL9535_STATE_WORK, XL9535_STATE_IDLE);
//           gpio->private.write_reg_addr = 2;
//         }
//         else {
//           HDL_REG_MODIFY(gpio->private.state, XL9535_STATE_WORK, XL9535_STATE_WRITE_SEL);
//         }
//       }
//       break;
//     }

//     case XL9535_STATE_IDLE:
//     default: {
//       if(gpio->private.state & XL9535_STATE_SYNC_R) {
//         HDL_REG_MODIFY(gpio->private.state, XL9535_STATE_WORK, XL9535_STATE_READ_SEL);
//         HDL_REG_CLEAR(gpio->private.state, XL9535_STATE_SYNC_R);
//       }
//       else if(gpio->private.state & XL9535_STATE_SYNC_W) {
//         HDL_REG_MODIFY(gpio->private.state, XL9535_STATE_WORK, XL9535_STATE_WRITE_SEL);
//         HDL_REG_CLEAR(gpio->private.state, XL9535_STATE_SYNC_W);
//       }
//       //todo read int
//       break;
//     }
//   }
//   return cancel;
// }

// hdl_module_state_t hdl_xl9535_port(void *desc, const uint8_t enable) {
//   hdl_xl9535_port_private_t *gpio = (hdl_xl9535_port_private_t *)desc;
//   if(enable) {
//     gpio->private.state = XL9535_STATE_SYNC_R | XL9535_STATE_IDLE;
//     gpio->private.input_port0 = 0x00;
//     gpio->private.input_port1 = 0x00;
//     gpio->private.output_port0 = gpio->private.user_output_port0 = 0xff;
//     gpio->private.output_port1 = gpio->private.user_output_port1 = 0xff;
//     gpio->private.polarity_port0 = gpio->private.user_polarity_port0 = 0x00;
//     gpio->private.polarity_port1 = gpio->private.user_polarity_port1 = 0x00;
//     gpio->private.config_port0 = gpio->private.user_config_port0 = 0xff;
//     gpio->private.config_port1 = gpio->private.user_config_port1 = 0xff;

//     gpio->private.read_reg_addr = 0;
//     gpio->private.write_reg_addr = 2;

//     gpio->private.i2c_msg.status = 0;
//     gpio->private.i2c_msg.buffer = &gpio->private.input_port0;
//     gpio->private.i2c_msg.address = (uint32_t)gpio->module.reg;

//     coroutine_add(&gpio->private.worker, &_xl9535_worker, (void*)gpio);
//     return HDL_MODULE_ACTIVE;
//   }
//   coroutine_cancel(&gpio->private.worker);
//   return HDL_MODULE_UNLOADED;
// }

// hdl_module_state_t hdl_xl9535_pin(void *desc, const uint8_t enable) {
//   hdl_xl9535_pin_t *pin = (hdl_xl9535_pin_t *)desc;
//   if(enable) {
//     hdl_xl9535_port_private_t *gpio = (hdl_xl9535_port_private_t *)pin->module.dependencies[0];
//     uint32_t pin_no = (uint32_t)pin->module.reg;
//     uint8_t *mode = &gpio->private.user_config_port0;
//     uint8_t *output = &gpio->private.user_output_port0;
//     uint8_t *pol = &gpio->private.user_polarity_port0;
//     if(pin_no >= 8) {
//       mode = &gpio->private.user_config_port1;
//       output = &gpio->private.user_output_port1;
//       pol = &gpio->private.user_polarity_port1;
//       pin_no -= 8;
//     }
//     uint8_t mask = 1 << pin_no;
//     HDL_REG_MODIFY(*mode, mask, (pin->config->mode << pin_no));
//     HDL_REG_MODIFY(*output, mask, (pin->config->output << pin_no));
//     HDL_REG_MODIFY(*pol, mask, (pin->config->polarity << pin_no));
//     gpio->private.state |= XL9535_STATE_SYNC_W;
//     return HDL_MODULE_ACTIVE;
//   }
//   return HDL_MODULE_UNLOADED;
// }

// static hdl_gpio_state _hdl_xl9535_read(const hdl_xl9535_pin_t *pin, uint8_t io) {
//   if (hdl_state(&pin->module) == HDL_MODULE_FAULT)
//     return HDL_GPIO_LOW;
//   hdl_xl9535_port_private_t *gpio = (hdl_xl9535_port_private_t *)pin->module.dependencies[0];
//   uint32_t pin_no = (uint32_t)pin->module.reg;
//   uint8_t *reg = io? &gpio->private.input_port0: &gpio->private.output_port0;
//   if(pin_no >= 8) {
//     reg = io? &gpio->private.input_port1: &gpio->private.output_port1;
//     pin_no -= 8;
//   }
//   uint8_t mask = 1 << pin_no;
//   return (*reg & mask)? HDL_GPIO_HIGH: HDL_GPIO_LOW;
// }


// hdl_gpio_state hdl_xl9535_read(const hdl_xl9535_pin_t *pin) {
//   return _hdl_xl9535_read(pin, 1);
// }

// hdl_gpio_state hdl_xl9535_read_output(const hdl_xl9535_pin_t *pin) {
//   return _hdl_xl9535_read(pin, 0);
// }

// static void _hdl_xl9535_write(const hdl_xl9535_pin_t *pin, const hdl_gpio_state state, uint8_t toggle) {
//   if (hdl_state(&pin->module) == HDL_MODULE_FAULT)
//     return;
//   hdl_xl9535_port_private_t *gpio = (hdl_xl9535_port_private_t *)pin->module.dependencies[0];
//   uint32_t pin_no = (uint32_t)pin->module.reg;
//   uint8_t *reg = &gpio->private.user_output_port0;
//   if(pin_no >= 8) {
//     reg = &gpio->private.user_output_port1;
//     pin_no -= 8;
//   }
//   uint8_t mask = 1 << pin_no;
//   uint8_t val;
//   if(toggle) val = *reg ^ mask;
//   else val = (state == HDL_GPIO_HIGH)? mask: 0;
//   if(*reg ^ val) gpio->private.state |= XL9535_STATE_SYNC_W;
//   HDL_REG_MODIFY(*reg, mask, val);
// }

// void hdl_xl9535_write(const hdl_xl9535_pin_t *pin, const hdl_gpio_state state) {
//   _hdl_xl9535_write(pin, state, 0);
// }

// void hdl_xl9535_toggle(const hdl_xl9535_pin_t *pin) {
//   _hdl_xl9535_write(pin, 0, 1);
// }
