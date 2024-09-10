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

HDL_ASSERRT_STRUCTURE_CAST(hdl_adc_ms5194t_private_t, hdl_adc_ms5194t_t, HDL_ADC_MS5194T_PRV_SIZE, ms5194t.h);

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
          adc->command_state &= ~MS5194T_ADC_COMMAND_STATE_CS;
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
          adc->message.tx_len |= (adc->command_state & MS5194T_ADC_COMMAND_STATE_XFER_DATA_32)? 4: 0;
          adc->message.rx_take = adc->message.tx_len;
          adc->message.tx_buffer = (uint8_t *)&adc->tx_data;
          adc->message.rx_buffer = (uint8_t *)&adc->rx_data;
          adc->command_state &= ~(MS5194T_ADC_COMMAND_STATE_XFER_DATA_MSK | MS5194T_ADC_COMMAND_STATE_CS_RELEASE);
        }
      }
    }
  }
}

#define MS5194T_ADC_STATE_RESET                    0
#define MS5194T_ADC_STATE_SET_MODE                 1
#define MS5194T_ADC_STATE_SET_IO                   2
#define MS5194T_ADC_STATE_SET_CONFIG               3
#define MS5194T_ADC_STATE_VALIDATE_CONFIG          4
#define MS5194T_ADC_STATE_SET_SINGLE_CONV          5
#define MS5194T_ADC_STATE_GET_STATUS               6
#define MS5194T_ADC_STATE_CHECK_READY              7
#define MS5194T_ADC_STATE_UPDATE_VALUE             8

#define MS5194T_ADC_STATE_INIT                     MS5194T_ADC_STATE_RESET


#define _adc_ms5194t_msb_u8_data(x)              (x & 0xff)
#define _adc_ms5194t_msb_u16_data(x)             (((x << 8) & 0xff00) | ((x >> 8) & 0x00ff))
#define _adc_ms5194t_msb_u24_data(x)             (((x >> 16) & 0x0000ff) | (x & 0x00ff00) | ((x << 16) & (0xff0000)))

//#define _adc_ms5194t_u8_data(data)                 (((uint32_t)data) >> 24)
//#define _adc_ms5194t_u16_data(data)                (((uint32_t)data) >> 16)
//#define _adc_ms5194t_u24_data(data)                (((uint32_t)data) >> 8)

static uint8_t _adc_ms5194t_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  hdl_adc_ms5194t_private_t *adc = (hdl_adc_ms5194t_private_t *)arg;
  if(adc->command_state != MS5194T_ADC_COMMAND_STATE_IDLE) {
    _adc_xfer(adc);
  }
  else {
    switch (adc->state) {
      case MS5194T_ADC_STATE_RESET: {
        adc->tx_data = 0xffffffff;
        adc->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_XFER_DATA_32 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
        adc->state = MS5194T_ADC_STATE_SET_MODE;
      }
      break;
      case MS5194T_ADC_STATE_SET_MODE: {
        adc->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_MODE_REG | !MS5194T_COMM_REG_CREAD;
        uint16_t mode = adc->config->mode_reg & ~MS5194T_MODE_REG_MS | MS5194T_MODE_REG_MS_PWR_DWN;
        adc->tx_data = _adc_ms5194t_msb_u16_data(mode);
        adc->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
          MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
        adc->state = MS5194T_ADC_STATE_SET_IO;
        break;
      }

      case MS5194T_ADC_STATE_SET_IO:
        adc->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_IO_REG | !MS5194T_COMM_REG_CREAD;
        adc->tx_data = _adc_ms5194t_msb_u8_data(adc->config->io_reg);
        adc->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
          MS5194T_ADC_COMMAND_STATE_XFER_DATA_8 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
        adc->state = MS5194T_ADC_STATE_SET_CONFIG;
        break;
      
      case MS5194T_ADC_STATE_SET_CONFIG:
        if(adc->src_count > 0) {
          adc->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_CONFIG_REG | !MS5194T_COMM_REG_CREAD;
          uint16_t cnf = adc->config->sources[adc->src_current]->config_reg;
          adc->tx_data = _adc_ms5194t_msb_u16_data(cnf);
          adc->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
            MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
          adc->state = MS5194T_ADC_STATE_VALIDATE_CONFIG;
        }
        break;

      case MS5194T_ADC_STATE_VALIDATE_CONFIG:
        adc->comm_reg = !MS5194T_COMM_REG_WEN | MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_CONFIG_REG | !MS5194T_COMM_REG_CREAD;
        uint16_t cnf = adc->config->sources[adc->src_current]->config_reg;
        adc->tx_data = 0;
        adc->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
          MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
        adc->state = MS5194T_ADC_STATE_SET_SINGLE_CONV;
        break;

      case MS5194T_ADC_STATE_SET_SINGLE_CONV: {
        uint16_t cnf = adc->rx_data;
        cnf = _adc_ms5194t_msb_u16_data(cnf);
        if(cnf == adc->config->sources[adc->src_current]->config_reg) {
          adc->comm_reg = !MS5194T_COMM_REG_WEN | !MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_MODE_REG | !MS5194T_COMM_REG_CREAD;
          uint16_t mode = adc->config->mode_reg & ~MS5194T_MODE_REG_MS | MS5194T_MODE_REG_MS_SINGLE;
          adc->tx_data = _adc_ms5194t_msb_u16_data(mode);
          adc->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
            MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
          adc->state = MS5194T_ADC_STATE_GET_STATUS;
        }
        else {
          adc->state = MS5194T_ADC_STATE_SET_CONFIG;
        }
        break;
      }


      case MS5194T_ADC_STATE_GET_STATUS:
        adc->comm_reg = !MS5194T_COMM_REG_WEN | MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_STATUS_REG | !MS5194T_COMM_REG_CREAD;
        adc->tx_data = 0;
        adc->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
          MS5194T_ADC_COMMAND_STATE_XFER_DATA_8 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
        adc->state = MS5194T_ADC_STATE_CHECK_READY;
        break;

      case MS5194T_ADC_STATE_CHECK_READY: {
        adc->state = MS5194T_ADC_STATE_GET_STATUS;
        uint8_t status = adc->rx_data;
        if(!(status & MS5194T_STATUS_REG_NOT_RDY)) {
          if(status & MS5194T_STATUS_REG_ERR) {
            adc->rx_data = HDL_ADC_MS5194T_INVALID_VALUE;
          }
          else {
            adc->comm_reg = !MS5194T_COMM_REG_WEN | MS5194T_COMM_REG_READ | MS5194T_COMM_REG_RS_DATA_REG | !MS5194T_COMM_REG_CREAD;
            adc->tx_data = 0;
            adc->command_state = MS5194T_ADC_COMMAND_STATE_CS | MS5194T_ADC_COMMAND_STATE_TX_COMM_REG | 
              MS5194T_ADC_COMMAND_STATE_XFER_DATA_16 | MS5194T_ADC_COMMAND_STATE_CS_RELEASE;
            if(status & MS5194T_STATUS_REG_SR3) 
              adc->command_state |= MS5194T_ADC_COMMAND_STATE_XFER_DATA_8; //if AD7794: read 24bit reg
          }
          adc->state = MS5194T_ADC_STATE_UPDATE_VALUE;
        }
        break;
      }

      case MS5194T_ADC_STATE_UPDATE_VALUE: {
        uint32_t data = adc->rx_data;
        data = _adc_ms5194t_msb_u24_data(data);
        volatile uint32_t volt = (250 * data) / 256; // 2.5V  scale 100
        volt = volt * 250 / 256; // scale 25000
        volt = volt * 4 / 256; // scale 100000
        volt /= 100;
        adc->config->values[adc->src_current] = data;
        adc->src_current++;
        if(adc->src_current >= adc->src_count) {
          adc->src_current = 0;
          hdl_time_counter_t *time = (hdl_time_counter_t *)adc->module.dependencies[1];
          adc->timestamp = hdl_time_counter_get(time);
        }
        adc->state = MS5194T_ADC_STATE_SET_CONFIG;
        break;
      }

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
      hdl_adc_ms5194t_source_t **src = adc->config->sources;
      while (*src != NULL) {
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
