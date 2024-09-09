#include "app.h"
#include "CodeLib.h"

#define MS5194T_RX_MESSAGE_SIZE    3
#define MS5194T_TX_MESSAGE_SIZE    4

typedef struct {
  hdl_module_t module;
  const hdl_adc_ms5194t_config_t *config;
  coroutine_t worker;
  uint32_t timestamp;
  uint8_t state;
  uint8_t command_state;
  uint8_t src_count;
  uint8_t src_current;
  uint32_t rx_data;
  uint32_t tx_data;
  uint8_t comm_reg;
  hdl_spi_message_t message;
} hdl_adc_ms5194t_private_t;

ASSERRT_STRUCTURE_CAST(hdl_adc_ms5194t_private_t, hdl_adc_ms5194t_t, HDL_ADC_MS5194T_PRV_SIZE, ms5194t.h);

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

static void _adc_xfer(hdl_adc_ms5194t_private_t *adc) {
  while(adc->command_state != MS5194T_ADC_COMMAND_STATE_IDLE) {
    if(adc->command_state & MS5194T_ADC_COMMAND_STATE_PUSH_MESSAGE) {
      hdl_spi_client_ch_t *spi = (hdl_spi_client_ch_t *)adc->module.dependencies[0];
      if(hdl_spi_transfer_message(spi, &adc->message)) {
        adc->command_state &= ~MS5194T_ADC_COMMAND_STATE_PUSH_MESSAGE;
        adc->command_state |= MS5194T_ADC_COMMAND_STATE_MESSAGE_AWAITING;
      }
      break;
    }
    else {
      if(adc->command_state & MS5194T_ADC_COMMAND_STATE_MESSAGE_AWAITING) {
        if(!(adc->message.state & HDL_SPI_MESSAGE_STATUS_COMPLETE)) break;
        adc->command_state &= ~MS5194T_ADC_COMMAND_STATE_MESSAGE_AWAITING;
      }
      else {
        adc->message.rx_skip = 0;
        adc->message.rx_take = 0;
        adc->message.tx_len = 0;
        adc->command_state |= MS5194T_ADC_COMMAND_STATE_PUSH_MESSAGE;
        if(adc->command_state & MS5194T_ADC_COMMAND_STATE_CS) {
          adc->message.options = HDL_SPI_MESSAGE_CH_SELECT;
          adc->command_state &= ~MS5194T_ADC_COMMAND_STATE_MESSAGE_AWAITING;
        }
        else if(adc->command_state & MS5194T_ADC_COMMAND_STATE_TX_COMM_REG) {
          adc->message.options = 0;
          adc->message.tx_len = 1;
          adc->message.tx_buffer = (uint8_t *)&adc->comm_reg;
          adc->command_state &= ~MS5194T_ADC_COMMAND_STATE_TX_COMM_REG;
        }
        else if(adc->command_state & MS5194T_ADC_COMMAND_STATE_XFER_DATA_MSK) {
          adc->message.options = HDL_SPI_MESSAGE_CH_RELEASE;
          adc->message.tx_len = (adc->command_state & MS5194T_ADC_COMMAND_STATE_XFER_DATA_8)? 1: 0;
          adc->message.tx_len |= (adc->command_state & MS5194T_ADC_COMMAND_STATE_XFER_DATA_16)? 2: 0;
          adc->message.tx_len = (adc->command_state & MS5194T_ADC_COMMAND_STATE_XFER_DATA_32)? 4: 0;
          adc->message.rx_take = adc->message.tx_len;
          adc->message.tx_buffer = (uint8_t *)&adc->tx_data;
          adc->message.rx_buffer = (uint8_t *)&adc->rx_data;
          adc->command_state &= ~MS5194T_ADC_COMMAND_STATE_XFER_DATA_MSK;
        }
      }
    }
  }
}

#define MS5194T_ADC_STATE_INIT                     0
#define MS5194T_ADC_STATE_INIT_MODE                MS5194T_ADC_STATE_INIT
#define MS5194T_ADC_STATE_INIT_IO                  1
#define MS5194T_ADC_STATE_SET_CONFIG               2
#define MS5194T_ADC_STATE_SET_SINGLE_CONV          3
#define MS5194T_ADC_STATE_GET_STATUS               4
#define MS5194T_ADC_STATE_CHECK_READY              5

#define _adc_ms5194t_to_u8_data(data)              (((uint32_t)data) << 24)
#define _adc_ms5194t_to_u16_data(data)             (((uint32_t)data) << 16)
#define _adc_ms5194t_to_u24_data(data)             (((uint32_t)data) << 8)

static uint8_t _adc_ms5194t_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  hdl_adc_ms5194t_private_t *adc = (hdl_adc_ms5194t_private_t *)arg;
  if(adc->command_state != MS5194T_ADC_COMMAND_STATE_IDLE) {
    _adc_xfer(adc);
  }
  else {
    switch (adc->state) {
      case MS5194T_ADC_STATE_INIT_MODE:
        adc->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_MODE_REG | !MS5194T_COMM_REG_CREAD;
        adc->tx_data = _adc_ms5194t_to_u16_data(adc->config->mode_reg);
        adc->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
          MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
        adc->state = MS5194T_ADC_STATE_INIT_IO;
        break;
      case MS5194T_ADC_STATE_INIT_IO:
        adc->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_IO_REG | !MS5194T_COMM_REG_CREAD;
        adc->tx_data = _adc_ms5194t_to_u8_data(adc->config->io_reg);
        adc->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
          MS5194T_ADC_COMMAND_STATE_XFER_DATA_8 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
        adc->state = MS5194T_ADC_STATE_SET_CONFIG;
        break;
      
      case MS5194T_ADC_STATE_SET_CONFIG:
        if(adc->src_count > 0) {
          adc->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_CONFIG_REG | !MS5194T_COMM_REG_CREAD;
          adc->tx_data = _adc_ms5194t_to_u16_data(adc->config->sources[adc->src_current]->config_reg);
          adc->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
            MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
          adc->state = MS5194T_ADC_STATE_SET_SINGLE_CONV;
        }
        break;
      
      case MS5194T_ADC_STATE_SET_SINGLE_CONV:
        if(adc->src_count > 0) {
          adc->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_CONFIG_REG | !MS5194T_COMM_REG_CREAD;
          adc->tx_data = adc->config->sources[adc->src_current]->config_reg;
          adc->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
            MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;

          adc->state = MS5194T_ADC_STATE_GET_STATUS;
        }
        break;


        case MS5194T_ADC_STATE_GET_STATUS:
          adc->state = MS5194T_ADC_STATE_CHECK_READY;
        break;

        case MS5194T_ADC_STATE_CHECK_READY:
          // if ready
          adc->state = MS5194T_ADC_STATE_SET_CONFIG;
          // else
          adc->state = MS5194T_ADC_STATE_GET_STATUS;
        break;

      default:
        break;
    }
  }
  return cancel;
}

hdl_module_state_t hdl_adc_ms5194t(void *desc, uint8_t enable) {
  hdl_adc_ms5194t_private_t *adc = (hdl_adc_ms5194t_private_t *)desc;
  if(enable) {
    adc->state = MS5194T_ADC_STATE_INIT;
    adc->timestamp = 0;
    adc->src_count = 0;
    adc->src_current = 0;
    adc->command_state = MS5194T_ADC_COMMAND_STATE_IDLE;
    if(adc->config->sources != NULL) {
      hdl_adc_ms5194t_source_t *src = *adc->config->sources;
      while (src != NULL) {
        adc->config->values[adc->src_count] = HDL_ADC_MS5194T_INVALID_VALUE;
        adc->src_count++;
        src++;
      }
    }
    coroutine_add(&adc->worker, &_adc_ms5194t_worker, desc);
    return HDL_MODULE_ACTIVE;
  }
  return HDL_MODULE_ACTIVE;
}

uint32_t hdl_adc_ms5194t_get(hdl_adc_ms5194t_t *desc, uint32_t src) {
  hdl_adc_ms5194t_private_t *adc = (hdl_adc_ms5194t_private_t *)desc;
  if((adc != NULL) && (hdl_state(&adc->module) == HDL_MODULE_ACTIVE) && (adc->src_count > src)) {
    return adc->config->values[src];
  }
  return HDL_ADC_MS5194T_INVALID_VALUE;
}

uint32_t hdl_adc_ms5194t_age(hdl_adc_ms5194t_t *desc) {
  hdl_adc_ms5194t_private_t *adc = (hdl_adc_ms5194t_private_t *)desc;
  if((adc != NULL) && (hdl_state(&adc->module) == HDL_MODULE_ACTIVE)) {
    return adc->timestamp;
  }
  return 0;
}
