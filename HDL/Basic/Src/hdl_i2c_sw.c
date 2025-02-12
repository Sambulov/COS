#include "hdl_iface.h"
#include "CodeLib.h"

typedef struct {
  coroutine_t i2c_worker;
  hdl_i2c_message_t *message;
  uint8_t wrk_state;
  uint8_t is_master   :1,
          addr        :1,
          dev_ack     :1,
          is_receiver :1;
  uint8_t ch_amount;
  hdl_transceiver_t *transceiver;
} hdl_i2c_sw_var_t;

//HDL_ASSERRT_STRUCTURE_CAST(hdl_i2c_sw_var_t, *((hdl_i2c_t *)0)->obj_var, HDL_I2C_SW_VAR_SIZE, hdl_button.h);

#define WRK_STATE_START    0
#define WRK_STATE_ADDR     1
#define WRK_STATE_DATA     2
#define WRK_STATE_STOP     3
#define WRK_STATE_COMPLETE 4
#define HDL_I2C_MESSAGE_FAULT_MASK 0x7F00

#define WC_STATE_AWAITING    0
#define WC_STATE_TIMEOUT    -1
#define WC_STATE_HIT         1

void _sw_i2c_line_switch_delay(hdl_i2c_t *i2c) {
  hdl_tick_counter_t *tc = (hdl_tick_counter_t *)i2c->dependencies[2];
  uint32_t ticks = hdl_tick_counter_get(tc);
  hdl_i2c_sw_hw_config_t *hwc = (hdl_i2c_sw_hw_config_t *)i2c->config->hwc;
  while ((hdl_tick_counter_get(tc) - ticks) < hwc->switch_delay);
}

static uint8_t _sw_i2c_start(hdl_i2c_t *i2c) {
  hdl_i2c_sw_var_t *i2c_var = (hdl_i2c_sw_var_t *)i2c->obj_var;
  if(i2c_var->message->options & HDL_I2C_MESSAGE_START) {
    if(!i2c_var->is_master || i2c_var->addr) {
      hdl_gpio_pin_t *scl = (hdl_gpio_pin_t *)i2c->dependencies[0];
      hdl_gpio_pin_t *sda = (hdl_gpio_pin_t *)i2c->dependencies[1];
      hdl_gpio_set_inactive(sda);
      _sw_i2c_line_switch_delay(i2c);
      hdl_gpio_set_inactive(scl);
      _sw_i2c_line_switch_delay(i2c);
      hdl_gpio_set_active(sda);
      _sw_i2c_line_switch_delay(i2c);
      hdl_gpio_set_active(scl);
      _sw_i2c_line_switch_delay(i2c);
      i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_START_ON_BUS;
      i2c_var->addr = 0;
      i2c_var->dev_ack = 0;
      i2c_var->is_master = 1;
    }
    else 
      i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
  }
  return HDL_TRUE;
}

uint8_t _sw_i2c_stop_handler(hdl_i2c_t *i2c) {
  hdl_i2c_sw_var_t *i2c_var = (hdl_i2c_sw_var_t *)i2c->obj_var;
  if(i2c_var->message->options & HDL_I2C_MESSAGE_STOP) {
    hdl_gpio_pin_t *scl = (hdl_gpio_pin_t *)i2c->dependencies[0];
    hdl_gpio_pin_t *sda = (hdl_gpio_pin_t *)i2c->dependencies[1];
    hdl_gpio_set_active(sda);
    _sw_i2c_line_switch_delay(i2c);
    hdl_gpio_set_inactive(scl);
    _sw_i2c_line_switch_delay(i2c);
    hdl_gpio_set_inactive(sda);
    _sw_i2c_line_switch_delay(i2c);
    i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_STOP_ON_BUS;
    i2c_var->is_master = 0;
    i2c_var->addr = 0;
    i2c_var->dev_ack = 0;
  }
  return HDL_TRUE;
}

uint8_t _sw_i2c_send_byte(hdl_i2c_t *i2c, uint8_t data) {
  hdl_gpio_pin_t *scl = (hdl_gpio_pin_t *)i2c->dependencies[0];
  hdl_gpio_pin_t *sda = (hdl_gpio_pin_t *)i2c->dependencies[1];
  for (int i = 7; i >= 0; i--) {
    if((data >> i) & 0x01) hdl_gpio_set_inactive(sda);
    else hdl_gpio_set_active(sda);
    _sw_i2c_line_switch_delay(i2c);
    hdl_gpio_set_inactive(scl);
    _sw_i2c_line_switch_delay(i2c);
    hdl_gpio_set_active(scl);
    _sw_i2c_line_switch_delay(i2c);
  }
  hdl_gpio_set_inactive(sda);
  _sw_i2c_line_switch_delay(i2c);
  hdl_gpio_set_inactive(scl);
  _sw_i2c_line_switch_delay(i2c);
  uint8_t ack = hdl_gpio_is_active(sda);
  hdl_gpio_set_active(scl);
  _sw_i2c_line_switch_delay(i2c);
  return ack;
}

uint8_t _sw_i2c_read_byte(hdl_i2c_t *i2c, uint8_t ack) {
  hdl_gpio_pin_t *scl = (hdl_gpio_pin_t *)i2c->dependencies[0];
  hdl_gpio_pin_t *sda = (hdl_gpio_pin_t *)i2c->dependencies[1];
  uint8_t data = 0;
  for (int i = 7; i >= 0; i--) {
    hdl_gpio_set_inactive(scl);
    _sw_i2c_line_switch_delay(i2c);
    data |= (hdl_gpio_is_inactive(sda) << i);
    hdl_gpio_set_active(scl);
    _sw_i2c_line_switch_delay(i2c);
  }
  if(ack) hdl_gpio_set_active(sda);
  else hdl_gpio_set_inactive(sda);
  _sw_i2c_line_switch_delay(i2c);
  hdl_gpio_set_inactive(scl);
  _sw_i2c_line_switch_delay(i2c);
  hdl_gpio_set_active(scl);
  _sw_i2c_line_switch_delay(i2c);
  hdl_gpio_set_inactive(sda);
  return data;
}

uint8_t _sw_i2c_addr_handler(hdl_i2c_t *i2c) {
  hdl_i2c_sw_var_t *i2c_var = (hdl_i2c_sw_var_t *)i2c->obj_var;
  if(i2c_var->message->options & HDL_I2C_MESSAGE_ADDR) {
    if((i2c_var->is_master) && (!i2c_var->addr)) {
      uint8_t data = i2c_var->message->address << 1;
      if(i2c_var->message->options & HDL_I2C_MESSAGE_MRSW) data |= 1;
      i2c_var->is_master = HDL_TRUE;
      if(_sw_i2c_send_byte(i2c, data)) {
        i2c_var->dev_ack = 1;
        i2c_var->is_receiver = (i2c_var->message->options & HDL_I2C_MESSAGE_MRSW) != 0;
      }
      i2c_var->addr = 1;
      i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_ADDR_SENT;
    }
    else
      i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
  }
  return HDL_TRUE;
}

uint8_t _sw_i2c_data_handler(hdl_i2c_t *i2c) {
  hdl_i2c_sw_var_t *i2c_var = (hdl_i2c_sw_var_t *)i2c->obj_var;
  if((i2c_var->message->length > 0) && (i2c_var->message->buffer != NULL)) {
    if(i2c_var->dev_ack) {
      if(i2c_var->message->transferred < i2c_var->message->length) {
        if(i2c_var->is_receiver) {
          uint8_t ack = 1;
          if((i2c_var->message->options & HDL_I2C_MESSAGE_NACK_LAST) && (i2c_var->message->transferred == (i2c_var->message->length - 1)))
            ack = 0;
          i2c_var->message->buffer[i2c_var->message->transferred] = _sw_i2c_read_byte(i2c, ack);
          if(!ack) i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_NACK;
          i2c_var->message->transferred++;
        }
        else {
          if(!_sw_i2c_send_byte(i2c, i2c_var->message->buffer[i2c_var->message->transferred])) {
            i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_NACK;
            return HDL_TRUE;
          }
          else i2c_var->message->transferred++;
        }
        return HDL_FALSE;
      }
    }
    else
      i2c_var->message->status |= HDL_I2C_MESSAGE_FAULT_BAD_STATE;
  }
  return HDL_TRUE;
}

static uint8_t _i2c_client_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_i2c_t *i2c = (hdl_i2c_t *)arg;
  hdl_i2c_sw_var_t *i2c_var = (hdl_i2c_sw_var_t *)i2c->obj_var;
  if(i2c_var->message != NULL) {
    switch (i2c_var->wrk_state) {
      case WRK_STATE_START:
        if(_sw_i2c_start(i2c))
          i2c_var->wrk_state++;
        break;
      case WRK_STATE_ADDR:
        if(_sw_i2c_addr_handler(i2c)) 
          i2c_var->wrk_state++;
        break;
      case WRK_STATE_DATA:
          if(_sw_i2c_data_handler(i2c)) 
            i2c_var->wrk_state++;
        break;
      case WRK_STATE_STOP:
        _sw_i2c_stop_handler(i2c);
        i2c_var->wrk_state++;
        break;
      case WRK_STATE_COMPLETE:
        i2c_var->message->status |= HDL_I2C_MESSAGE_STATUS_COMPLETE;
        i2c_var->message = NULL;
        break;
      default:
        break;
    }
    if((i2c_var->message->status & HDL_I2C_MESSAGE_FAULT_MASK) && (i2c_var->wrk_state != WRK_STATE_COMPLETE)) 
      i2c_var->wrk_state = WRK_STATE_STOP;
  }
  return cancel;
}

static hdl_module_state_t _hdl_i2c_sw(const void *desc, uint8_t enable) {
  hdl_i2c_t *i2c = (hdl_i2c_t *)desc;
  hdl_i2c_sw_var_t *i2c_var = (hdl_i2c_sw_var_t *)i2c->obj_var;
  if(enable) {
    coroutine_add(&i2c_var->i2c_worker, &_i2c_client_worker, (void *)i2c);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&i2c_var->i2c_worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_i2c_transfer(const void *desc, hdl_i2c_message_t *message) {
  hdl_i2c_t *i2c = (hdl_i2c_t *)desc;
  hdl_i2c_sw_var_t *i2c_var = (hdl_i2c_sw_var_t *)i2c->obj_var;
  if((hdl_state(i2c) != HDL_MODULE_UNLOADED) && (i2c_var->message == NULL)) {
    message->status = 0;
    message->transferred = 0;
    i2c_var->message = message;
    i2c_var->wrk_state = WRK_STATE_START;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _hdl_i2c_transceiver_set(const void *desc, hdl_transceiver_t *transceiver, uint32_t channel_id) {
  hdl_i2c_t *i2c = (hdl_i2c_t *)desc;
  if(i2c->config->channels != NULL) {
    uint32_t ch = 0;
    while (ch <= channel_id) {
      if(i2c->config->channels[ch] == NULL) return HDL_FALSE;
      ch++;
    }
    ch--;
    i2c->config->channels[ch]->transceiver = transceiver;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

const hdl_i2c_iface_t hdl_i2c_sw_iface = {
  .init = &_hdl_i2c_sw,
  .transfer = &_hdl_i2c_transfer,
  .transceiver_set = &_hdl_i2c_transceiver_set,
};
