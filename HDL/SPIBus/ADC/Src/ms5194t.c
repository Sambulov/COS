#include "hdl_iface.h"

#define MS5194T_RX_MESSAGE_SIZE    3
#define MS5194T_TX_MESSAGE_SIZE    4

typedef struct {
  coroutine_t worker;
  uint32_t timestamp;
  uint8_t state;
  uint8_t command_state;
  uint8_t src_count;
  uint8_t src_current;
  uint32_t rx_data;
  uint32_t tx_data;
  uint8_t comm_reg;
  uint8_t src_calibrated;
  hdl_spi_message_t message;
} hdl_adc_ms5194t_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_adc_ms5194t_var_t, *((hdl_adc_ms5194t_t *)0)->obj_var, HDL_ADC_MS5194T_VAR_SIZE, ms5194t.h);

#define MS5194T_ADC_COMMAND_STATE_IDLE                  0
#define MS5194T_ADC_COMMAND_STATE_CS                    1
#define MS5194T_ADC_COMMAND_STATE_CS_RELEASE            2
#define MS5194T_ADC_COMMAND_STATE_TX_COMM_REG           4
#define MS5194T_ADC_COMMAND_STATE_XFER_DATA_8           8
#define MS5194T_ADC_COMMAND_STATE_XFER_DATA_16         16
#define MS5194T_ADC_COMMAND_STATE_XFER_DATA_32         32
#define MS5194T_ADC_COMMAND_STATE_XFER_DATA_MSK        56
#define MS5194T_ADC_COMMAND_STATE_PUSH_MESSAGE         64
#define MS5194T_ADC_COMMAND_STATE_MESSAGE_AWAITING    128

static void _adc_xfer(hdl_adc_ms5194t_t *adc) {
  hdl_adc_ms5194t_var_t *adc_var = (hdl_adc_ms5194t_var_t *)adc->obj_var;
  while(adc_var->command_state != MS5194T_ADC_COMMAND_STATE_IDLE) {
    if(adc_var->command_state & MS5194T_ADC_COMMAND_STATE_PUSH_MESSAGE) {
      hdl_spi_client_ch_t *spi = (hdl_spi_client_ch_t *)adc->dependencies[0];
      if(hdl_spi_transfer_message(spi, &adc_var->message)) {
        adc_var->command_state &= ~MS5194T_ADC_COMMAND_STATE_PUSH_MESSAGE;
        adc_var->command_state |= MS5194T_ADC_COMMAND_STATE_MESSAGE_AWAITING;
      }
      break;
    }
    else {
      if(adc_var->command_state & MS5194T_ADC_COMMAND_STATE_MESSAGE_AWAITING) {
        if(!(adc_var->message.state & HDL_SPI_MESSAGE_STATUS_COMPLETE)) break;
        adc_var->command_state &= ~MS5194T_ADC_COMMAND_STATE_MESSAGE_AWAITING;
      }
      else {
        adc_var->message.rx_skip = 0;
        adc_var->message.rx_take = 0;
        adc_var->message.tx_len = 0;
        adc_var->message.options = 0;
        adc_var->command_state |= MS5194T_ADC_COMMAND_STATE_PUSH_MESSAGE;
        if(adc_var->command_state & MS5194T_ADC_COMMAND_STATE_CS) {
          adc_var->message.options = HDL_SPI_MESSAGE_CH_SELECT;
          adc_var->command_state &= ~MS5194T_ADC_COMMAND_STATE_CS;
        }
        else if(adc_var->command_state & MS5194T_ADC_COMMAND_STATE_TX_COMM_REG) {
          adc_var->message.tx_len = 1;
          adc_var->message.tx_buffer = (uint8_t *)&adc_var->comm_reg;
          adc_var->command_state &= ~MS5194T_ADC_COMMAND_STATE_TX_COMM_REG;
        }
        else if(adc_var->command_state & MS5194T_ADC_COMMAND_STATE_XFER_DATA_MSK) {
          adc_var->message.tx_len = (adc_var->command_state & MS5194T_ADC_COMMAND_STATE_XFER_DATA_8)? 1: 0;
          adc_var->message.tx_len |= (adc_var->command_state & MS5194T_ADC_COMMAND_STATE_XFER_DATA_16)? 2: 0;
          adc_var->message.tx_len |= (adc_var->command_state & MS5194T_ADC_COMMAND_STATE_XFER_DATA_32)? 4: 0;
          adc_var->message.rx_take = adc_var->message.tx_len;
          adc_var->message.tx_buffer = (uint8_t *)&adc_var->tx_data;
          adc_var->message.rx_buffer = (uint8_t *)&adc_var->rx_data;
          adc_var->command_state &= ~MS5194T_ADC_COMMAND_STATE_XFER_DATA_MSK;
        }
        else if(adc_var->command_state & MS5194T_ADC_COMMAND_STATE_CS_RELEASE) {
          adc_var->command_state &= ~MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
          adc_var->message.options |= HDL_SPI_MESSAGE_CH_RELEASE;
        }

      }
    }
  }
}

#define MS5194T_ADC_TASK_INIT                     0x00
#define MS5194T_ADC_TASK_ZS_CALIBR                0x10
#define MS5194T_ADC_TASK_FS_CALIBR                0x20
#define MS5194T_ADC_TASK_CONVERT                  0x30
#define MS5194T_ADC_TASK_GET_VALUE                0x40
#define MS5194T_ADC_TASK_UPDATE_VALUE16           0x50
#define MS5194T_ADC_TASK_UPDATE_VALUE24           0x60
#define MS5194T_ADC_TASK_CALIB_COMPLETE           0x70

#define MS5194T_ADC_TASK_MASK                     0xF0

#define MS5194T_ADC_STATE_RESET                   0x00
#define MS5194T_ADC_STATE_SET_MODE                0x01
#define MS5194T_ADC_STATE_SET_IO                  0x02
#define MS5194T_ADC_STATE_SET_CH_CONFIG           0x03
#define MS5194T_ADC_STATE_GET_CH_CONFIG           0x04
#define MS5194T_ADC_STATE_VALIDATE_CONFIG         0x05
#define MS5194T_ADC_STATE_DO_WORK                 0x06
#define MS5194T_ADC_STATE_CS                      0x07
#define MS5194T_ADC_STATE_CHECK_READY             0x08
#define MS5194T_ADC_STATE_DEINIT                  0x09
#define MS5194T_ADC_STATE_UNLOAD                  0x0A

#define MS5194T_ADC_STATE_MASK                    0x0F

#define MS5194T_ADC_STATE_INIT                    MS5194T_ADC_TASK_INIT | MS5194T_ADC_STATE_RESET


#define _adc_ms5194t_msb_u8_data(x)              (x & 0xff)
#define _adc_ms5194t_msb_u16_data(x)             (((x << 8) & 0xff00) | ((x >> 8) & 0x00ff))
#define _adc_ms5194t_msb_u24_data(x)             (((x >> 16) & 0x0000ff) | (x & 0x00ff00) | ((x << 16) & (0xff0000)))

static uint8_t _adc_ms5194t_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_adc_ms5194t_t *adc = (hdl_adc_ms5194t_t *)arg;
  hdl_adc_ms5194t_var_t *adc_var = (hdl_adc_ms5194t_var_t *)adc->obj_var;
  if(adc_var->command_state != MS5194T_ADC_COMMAND_STATE_IDLE) {
    _adc_xfer(adc);
  }
  else {
    switch (adc_var->state & MS5194T_ADC_STATE_MASK) {
      case MS5194T_ADC_STATE_RESET: {
        adc_var->src_current = adc_var->src_count;
        adc_var->src_calibrated = 0;
        adc_var->tx_data = 0xffffffff;
        adc_var->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_XFER_DATA_32 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
        adc_var->state = MS5194T_ADC_STATE_SET_MODE;
        break;
      }

      case MS5194T_ADC_STATE_SET_MODE: {
        adc_var->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_MODE_REG | !MS5194T_COMM_REG_CREAD;
        uint16_t mode = (adc->config->mode_reg & ~MS5194T_MODE_REG_MS) | MS5194T_MODE_REG_MS_PWR_DWN;
        adc_var->tx_data = _adc_ms5194t_msb_u16_data(mode);
        adc_var->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
          MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
        adc_var->state = MS5194T_ADC_STATE_SET_IO;
        break;
      }

      case MS5194T_ADC_STATE_SET_IO: {
        adc_var->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_IO_REG | !MS5194T_COMM_REG_CREAD;
        adc_var->tx_data = _adc_ms5194t_msb_u8_data(adc->config->io_reg);
        adc_var->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
          MS5194T_ADC_COMMAND_STATE_XFER_DATA_8 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
        adc_var->state = MS5194T_ADC_STATE_SET_CH_CONFIG;
        break;
      }

      case MS5194T_ADC_STATE_SET_CH_CONFIG: {
        if(cancel) {
          adc_var->state = MS5194T_ADC_STATE_DEINIT;
          break;
        }
        if(adc_var->src_count > 0) {
          adc_var->src_current++;
          if(adc_var->src_current >= adc_var->src_count) {
            adc_var->src_current = 0;
            hdl_time_counter_t *time = (hdl_time_counter_t *)adc->dependencies[2];
            adc_var->timestamp = hdl_time_counter_get(time);
          }
          if(adc->config->sources[adc_var->src_current]->options & HDL_ADC_MS5194T_CHANNEL_ENABLE) {
            adc_var->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_CONFIG_REG | !MS5194T_COMM_REG_CREAD;
            uint16_t cnf = adc->config->sources[adc_var->src_current]->config_reg;
            adc_var->tx_data = _adc_ms5194t_msb_u16_data(cnf);
            adc_var->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
              MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
            adc_var->state = MS5194T_ADC_STATE_GET_CH_CONFIG;
          }
          else {
            if(adc_var->src_calibrated < (adc_var->src_current + 1)) {
              adc_var->src_calibrated = adc_var->src_current + 1;
            }
          }
        }
        break;
      }

      case MS5194T_ADC_STATE_GET_CH_CONFIG: {
        adc_var->comm_reg = (!MS5194T_COMM_REG_WEN) | MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_CONFIG_REG | (!MS5194T_COMM_REG_CREAD);
        adc_var->tx_data = 0;
        adc_var->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
          MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
        adc_var->state = MS5194T_ADC_STATE_VALIDATE_CONFIG;
        break;
      }

      case MS5194T_ADC_STATE_VALIDATE_CONFIG: {
        uint16_t cnf = adc_var->rx_data;
        cnf = _adc_ms5194t_msb_u16_data(cnf);
        if(cnf == adc->config->sources[adc_var->src_current]->config_reg) {
          adc_var->state = MS5194T_ADC_STATE_DO_WORK;
          adc_var->state |=  
            (adc_var->src_calibrated < adc_var->src_count)? MS5194T_ADC_TASK_ZS_CALIBR : MS5194T_ADC_TASK_CONVERT;
        }
        else {
          adc_var->state = MS5194T_ADC_STATE_INIT;
        }
        break;
      }

      case MS5194T_ADC_STATE_DO_WORK: {
        adc_var->state = (adc_var->state & ~MS5194T_ADC_STATE_MASK) | MS5194T_ADC_STATE_CS;
        if((adc_var->state & MS5194T_ADC_TASK_MASK) == MS5194T_ADC_TASK_CALIB_COMPLETE) {
          adc_var->state = (adc_var->state & ~MS5194T_ADC_STATE_MASK) | MS5194T_ADC_STATE_SET_CH_CONFIG;
          adc_var->src_calibrated = (adc_var->src_current + 1);
          break;
        }

        if(((adc_var->state & MS5194T_ADC_TASK_MASK) == MS5194T_ADC_TASK_UPDATE_VALUE16) ||
           ((adc_var->state & MS5194T_ADC_TASK_MASK) == MS5194T_ADC_TASK_UPDATE_VALUE24)) {
          
          adc_var->state = (adc_var->state & ~MS5194T_ADC_STATE_MASK) | MS5194T_ADC_STATE_SET_CH_CONFIG;
          uint32_t data = adc_var->rx_data;
          adc->config->values[adc_var->src_current] = ((adc_var->state & MS5194T_ADC_TASK_MASK) == MS5194T_ADC_TASK_UPDATE_VALUE24)? 
                                                  _adc_ms5194t_msb_u24_data(data):
                                                  _adc_ms5194t_msb_u16_data(data);
          break;
        }

        if((adc_var->state & MS5194T_ADC_TASK_MASK) == MS5194T_ADC_TASK_ZS_CALIBR) {
          adc_var->state = (adc_var->state & ~MS5194T_ADC_TASK_MASK) | MS5194T_ADC_TASK_FS_CALIBR;
          if(adc->config->sources[adc_var->src_current]->options & HDL_ADC_MS5194T_CHANNEL_ZERO_SCALE_CALIBRATE) {
            adc_var->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_MODE_REG | !MS5194T_COMM_REG_CREAD;
            uint16_t mode = (adc->config->mode_reg & ~MS5194T_MODE_REG_MS) | MS5194T_MODE_REG_MS_CAL_I_ZERO_SCALE;
            adc_var->tx_data = _adc_ms5194t_msb_u16_data(mode);
            adc_var->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
              MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
            break;
          }
        }

        if((adc_var->state & MS5194T_ADC_TASK_MASK) == MS5194T_ADC_TASK_FS_CALIBR) {
          adc_var->state = (adc_var->state & ~MS5194T_ADC_TASK_MASK) | MS5194T_ADC_TASK_CALIB_COMPLETE;
          if(adc->config->sources[adc_var->src_current]->options & HDL_ADC_MS5194T_CHANNEL_FULL_SCALE_CALIBRATE) {
            adc_var->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_MODE_REG | !MS5194T_COMM_REG_CREAD;
            uint16_t mode = (adc->config->mode_reg & ~MS5194T_MODE_REG_MS) | MS5194T_MODE_REG_MS_CAL_I_FULL_SCALE;
            adc_var->tx_data = _adc_ms5194t_msb_u16_data(mode);
            adc_var->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
              MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
            break;
          }
        }

        if((adc_var->state & MS5194T_ADC_TASK_MASK) == MS5194T_ADC_TASK_CONVERT) {
          adc_var->state = (adc_var->state & ~MS5194T_ADC_TASK_MASK) | MS5194T_ADC_TASK_GET_VALUE;
          adc_var->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_MODE_REG | !MS5194T_COMM_REG_CREAD;
          uint16_t mode = (adc->config->mode_reg & ~MS5194T_MODE_REG_MS) | MS5194T_MODE_REG_MS_SINGLE;
          adc_var->tx_data = _adc_ms5194t_msb_u16_data(mode);
          adc_var->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
            MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
          break;
        }

        if((adc_var->state & MS5194T_ADC_TASK_MASK) == MS5194T_ADC_TASK_GET_VALUE) {
          uint8_t status = adc_var->rx_data;
          if(status & MS5194T_STATUS_REG_ERR) {
            adc_var->rx_data = HDL_ADC_MS5194T_INVALID_VALUE;
          }
          else {
            adc_var->comm_reg = (!MS5194T_COMM_REG_WEN) | MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_DATA_REG | (!MS5194T_COMM_REG_CREAD);
            adc_var->tx_data = 0;
            adc_var->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
              MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
            if(status & MS5194T_STATUS_REG_SR3) 
              adc_var->command_state |= MS5194T_ADC_COMMAND_STATE_XFER_DATA_8; //if AD7794: read 24bit reg
          }
          adc_var->state = MS5194T_ADC_STATE_DO_WORK | 
            ((status & MS5194T_STATUS_REG_SR3)? MS5194T_ADC_TASK_UPDATE_VALUE24: MS5194T_ADC_TASK_UPDATE_VALUE16);;
        }
        break;
      }
      case MS5194T_ADC_STATE_CS: {
        adc_var->state = (adc_var->state & ~MS5194T_ADC_STATE_MASK) | MS5194T_ADC_STATE_CHECK_READY;
        adc_var->tx_data = 0;
        adc_var->rx_data = 0;
        adc_var->command_state = MS5194T_ADC_COMMAND_STATE_CS;
        break;
      }

      case MS5194T_ADC_STATE_CHECK_READY: {
        adc_var->command_state = MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
        adc_var->state = (adc_var->state & ~MS5194T_ADC_STATE_MASK) | MS5194T_ADC_STATE_CS;
        hdl_gpio_pin_t *rdy_pin = (hdl_gpio_pin_t *)adc->dependencies[1];
        if(hdl_gpio_read(rdy_pin) == HDL_GPIO_LOW) {
          adc_var->comm_reg = (!MS5194T_COMM_REG_WEN) | MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_STATUS_REG | (!MS5194T_COMM_REG_CREAD);
          adc_var->tx_data = 0;
          adc_var->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
            MS5194T_ADC_COMMAND_STATE_XFER_DATA_8 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
          adc_var->state = (adc_var->state & ~MS5194T_ADC_STATE_MASK) | MS5194T_ADC_STATE_DO_WORK;
        }
        break;
      }

      case MS5194T_ADC_STATE_DEINIT: {
        adc_var->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_MODE_REG | !MS5194T_COMM_REG_CREAD;
        uint16_t mode =  MS5194T_MODE_REG_MS_PWR_DWN | MS5194T_MODE_REG_FILTER_RATE(10);
        adc_var->tx_data = _adc_ms5194t_msb_u16_data(mode);
        adc_var->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
          MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
        adc_var->state = MS5194T_ADC_STATE_UNLOAD;
        break;
      }
      case MS5194T_ADC_STATE_UNLOAD:
      default:
        return HDL_TRUE;
    }
  }
  return HDL_FALSE;
}

static hdl_module_state_t _hdl_adc_ms5194t(const void *desc, uint8_t enable) {
  hdl_adc_ms5194t_t *adc = (hdl_adc_ms5194t_t *)desc;
  hdl_adc_ms5194t_var_t *adc_var = (hdl_adc_ms5194t_var_t *)adc->obj_var;
  if(enable) {
    adc_var->state = MS5194T_ADC_STATE_INIT;
    adc_var->timestamp = 0;
    adc_var->src_count = 0;
    adc_var->src_calibrated = 0;
    adc_var->command_state = MS5194T_ADC_COMMAND_STATE_IDLE;
    if(adc->config->sources != NULL) {
      hdl_adc_ms5194t_source_t **src = adc->config->sources;
      while (*src != NULL) {
        adc->config->values[adc_var->src_count] = HDL_ADC_MS5194T_INVALID_VALUE;
        adc_var->src_count++;
        src++;
      }
    }
    adc_var->src_current = adc_var->src_count;
    coroutine_add(&adc_var->worker, &_adc_ms5194t_worker, adc);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_state_t cor_state = coroutine_state(&adc_var->worker);
  if(cor_state == CO_ROUTINE_RUN) coroutine_cancel(&adc_var->worker);
  return (cor_state == CO_ROUTINE_CANCELED)? HDL_MODULE_UNLOADED: HDL_MODULE_UNLOADING;
}

static uint32_t _hdl_adc_ms5194t_get(const void *desc, uint32_t src) {
  hdl_adc_ms5194t_t *adc = (hdl_adc_ms5194t_t *)desc;
  hdl_adc_ms5194t_var_t *adc_var = (hdl_adc_ms5194t_var_t *)adc->obj_var;
  if((adc != NULL) && (hdl_state(adc) == HDL_MODULE_ACTIVE) && (adc_var->src_count > src)) {
    return adc->config->values[src];
  }
  return HDL_ADC_MS5194T_INVALID_VALUE;
}

static uint32_t _hdl_adc_ms5194t_age(const void *desc) {
  hdl_adc_ms5194t_t *adc = (hdl_adc_ms5194t_t *)desc;
  hdl_adc_ms5194t_var_t *adc_var = (hdl_adc_ms5194t_var_t *)adc->obj_var;
  if((adc != NULL) && (hdl_state(adc) == HDL_MODULE_ACTIVE)) {
    return adc_var->timestamp;
  }
  return 0;
}

hdl_adc_iface_t hdl_adc_ms5194t_iface = {
  .init = &_hdl_adc_ms5194t,
  .get = &_hdl_adc_ms5194t_get,
  .age = &_hdl_adc_ms5194t_age
};
