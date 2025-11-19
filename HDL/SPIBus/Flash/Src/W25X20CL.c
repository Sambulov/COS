
#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_spi_message_t msg;
  hdl_nvm_message_t *nvm_msg;
  uint8_t cmd_n_addr[4]; /* cmd + 24 bits addr */
  uint8_t op;
  uint8_t state  : 7,
          cancel : 1;
  uint32_t burn_time;
} hdl_w25x20cl_var_t;

HDL_ASSERRT_STRUCTURE_CAST(hdl_w25x20cl_var_t, *((hdl_w25x20cl_t *)0)->obj_var, HDL_W25X20CL_VAR_SIZE, "w25x20cl.h");

#define NVM_STATE_IDLE                  0
#define NVM_STATE_WP_DISABLE_MSG        1
#define NVM_STATE_TX_WP_MSG             2
#define NVM_STATE_AWAIT_WP_MSG          3
#define NVM_STATE_SET_CMD_MSG           4
#define NVM_STATE_TX_CMD_MSG            5
#define NVM_STATE_AWAIT_CMD_MSG         6
#define NVM_STATE_TX_DATA_MSG           7
#define NVM_STATE_AWAIT_DATA_MSG        8
#define NVM_STATE_AWAIT_BURNING         9
#define NVM_STATE_COMPLETE              10

#define FLASH_READ     1
#define FLASH_WRITE    2
#define FLASH_ERASE    3

static uint8_t _flash_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_w25x20cl_t *flash = (hdl_w25x20cl_t *)arg;
  hdl_w25x20cl_var_t *flash_var = (hdl_w25x20cl_var_t *)flash->obj_var;
  switch (flash_var->state) {
    case NVM_STATE_IDLE:
      if(flash_var->nvm_msg == NULL) break;
      hdl_nvm_message_t *nvm_msg = flash_var->nvm_msg;
      uint8_t rw_op_bad = (!flash_var->op) ||
      ((flash_var->op == FLASH_READ) && ((nvm_msg->data == NULL) || (nvm_msg->size == 0))) ||
      ((flash_var->op == FLASH_WRITE) && ((nvm_msg->data == NULL) || (nvm_msg->size == 0))) ||
      ((flash_var->op == FLASH_ERASE) && ((nvm_msg->size == 0) || (nvm_msg->address % flash->config->sector_size) || (nvm_msg->size % flash->config->sector_size)));
      hdl_nvm_message_status_t err = 0;
      if(rw_op_bad) err = HDL_NVM_ERROR_BAD_ARG;
      if((nvm_msg->address + nvm_msg->size) > flash->config->size) err = HDL_NVM_ERROR_OUT_OF_RANGE;
      if(err) {
        nvm_msg->out_status |= err;
        flash_var->state = NVM_STATE_COMPLETE;
        break;
      }
    /* fall through */
    case NVM_STATE_WP_DISABLE_MSG: {
      if(flash_var->op == FLASH_READ) {
        flash_var->state = NVM_STATE_SET_CMD_MSG;
        break;
      }
      hdl_gpio_pin_t *wp_pin = (hdl_gpio_pin_t *)flash->dependencies[2];
      hdl_gpio_set_inactive(wp_pin);
      flash_var->cmd_n_addr[0] = 0x06; /* write protect disable */
      flash_var->msg.rx_buffer = NULL;
      flash_var->msg.rx_skip = 1; /* 1 cmd */
      flash_var->msg.rx_take = 0;
      flash_var->msg.tx_buffer = flash_var->cmd_n_addr;
      flash_var->msg.tx_len = 1; /* 1 cmd */
      flash_var->msg.options = HDL_SPI_MESSAGE_CH_SELECT | HDL_SPI_MESSAGE_CH_RELEASE;
      flash_var->state = NVM_STATE_TX_WP_MSG;
    }
    /* fall through */
    case NVM_STATE_TX_WP_MSG: {
      hdl_spi_client_ch_t *bus = (hdl_spi_client_ch_t *)flash->dependencies[0];
      if(hdl_spi_client_ch_transfer(bus, &flash_var->msg)) flash_var->state = NVM_STATE_AWAIT_WP_MSG;
      break;
    }
    case NVM_STATE_AWAIT_WP_MSG: {
      hdl_gpio_pin_t *wp_pin = (hdl_gpio_pin_t *)flash->dependencies[2];
      if(hdl_gpio_is_active(wp_pin)) break;
      if(!(flash_var->msg.status & HDL_SPI_MESSAGE_STATUS_COMPLETE)) break;
      if(flash_var->msg.status & HDL_SPI_MESSAGE_FAULT_BUS_ERROR) {
        flash_var->nvm_msg->out_status = HDL_NVM_ERROR_BUS_FAULT;
        flash_var->state = NVM_STATE_COMPLETE;
        break;
      }
      flash_var->state = NVM_STATE_SET_CMD_MSG;
    }
    /* fall through */
    case NVM_STATE_SET_CMD_MSG: {
      flash_var->msg.options = HDL_SPI_MESSAGE_CH_SELECT;
      flash_var->msg.rx_skip = 4; /* 1 cmd + 3 addr */
      if(flash_var->op == FLASH_READ) {
        flash_var->cmd_n_addr[0] = 0x0b; /* fast read cmd */
        flash_var->msg.rx_skip = 5; /* 1 cmd + 3 addr + 1 dummy */
      }
      else if(flash_var->op == FLASH_WRITE) 
        flash_var->cmd_n_addr[0] = 0x02; /* page programm */
      else if(flash_var->op == FLASH_ERASE) {
        flash_var->cmd_n_addr[0] = 0x20; /* sector erase */
        flash_var->msg.options |= HDL_SPI_MESSAGE_CH_RELEASE;
      }
      uint32_t addr = flash_var->nvm_msg->address + flash_var->nvm_msg->synced_size;
      flash_var->cmd_n_addr[1] = addr >> 16;
      flash_var->cmd_n_addr[2] = addr >> 8;
      flash_var->cmd_n_addr[3] = addr;
      flash_var->msg.rx_buffer = NULL;
      flash_var->msg.rx_take = 0;
      flash_var->msg.tx_buffer = flash_var->cmd_n_addr;
      flash_var->msg.tx_len = 4; /* 1 cmd + 3 addr */
      flash_var->state = NVM_STATE_TX_CMD_MSG;
    }
    /* fall through */
    case NVM_STATE_TX_CMD_MSG: {
      hdl_spi_client_ch_t *bus = (hdl_spi_client_ch_t *)flash->dependencies[0]; 
      if(hdl_spi_client_ch_transfer(bus, &flash_var->msg)) flash_var->state = NVM_STATE_AWAIT_CMD_MSG;
      break;
    }
    case NVM_STATE_AWAIT_CMD_MSG: {
      if(!(flash_var->msg.status & HDL_SPI_MESSAGE_STATUS_COMPLETE)) break;
      if(flash_var->msg.status & HDL_SPI_MESSAGE_FAULT_BUS_ERROR) {
        flash_var->nvm_msg->out_status = HDL_NVM_ERROR_BUS_FAULT;
        flash_var->state = NVM_STATE_COMPLETE;
        break;
      }
      flash_var->msg.rx_buffer = flash_var->nvm_msg->data;
      flash_var->msg.rx_skip = 0;
      flash_var->msg.rx_take = flash_var->nvm_msg->size;
      flash_var->msg.tx_buffer = NULL;
      flash_var->msg.tx_len = 0;
      flash_var->msg.options = HDL_SPI_MESSAGE_CH_RELEASE;
      if(flash_var->op == FLASH_READ) 
        __NOP();
      else if(flash_var->op == FLASH_WRITE) {
        uint32_t len = flash_var->nvm_msg->size - flash_var->nvm_msg->synced_size;
        len = (len <= flash->config->page_size)? len: flash->config->page_size;
        flash_var->msg.rx_buffer = NULL;
        flash_var->msg.rx_skip = 0;
        flash_var->msg.rx_take = 0;
        flash_var->msg.tx_buffer = (flash_var->nvm_msg->data + flash_var->nvm_msg->synced_size);
        flash_var->msg.tx_len = (flash_var->nvm_msg->size - flash_var->nvm_msg->synced_size);
      }
      else if(flash_var->op == FLASH_ERASE) {
        flash_var->nvm_msg->synced_size += flash->config->sector_size;
        hdl_time_counter_t *time_cnt = (hdl_time_counter_t *)flash->dependencies[1];
        flash_var->burn_time = hdl_time_counter_get(time_cnt);
        flash_var->state = NVM_STATE_AWAIT_BURNING;
        break;
      }
      //else if (flash_var->op & HDL_NVM_OPTION_VALIDATE) __NOP();
      flash_var->state = NVM_STATE_TX_DATA_MSG;
    }
    /* fall through */
    case NVM_STATE_TX_DATA_MSG: {
      hdl_spi_client_ch_t *bus = (hdl_spi_client_ch_t *)flash->dependencies[0]; 
      if(hdl_spi_client_ch_transfer(bus, &flash_var->msg)) flash_var->state = NVM_STATE_AWAIT_DATA_MSG;
      break;
    }

    case NVM_STATE_AWAIT_DATA_MSG: {
      if(!(flash_var->msg.status & HDL_SPI_MESSAGE_STATUS_COMPLETE)) break;
      if(flash_var->msg.status & HDL_SPI_MESSAGE_FAULT_BUS_ERROR) {
        flash_var->nvm_msg->out_status = HDL_NVM_ERROR_BUS_FAULT;
        flash_var->state = NVM_STATE_COMPLETE;
        break;
      }
      hdl_nvm_message_t *nvm_msg = flash_var->nvm_msg;
      nvm_msg->synced_size += flash_var->msg.transferred;
      if(flash_var->op == FLASH_READ)
        __NOP();
      else if(flash_var->op == FLASH_WRITE) {
        flash_var->state = NVM_STATE_AWAIT_BURNING;
        hdl_time_counter_t *time_cnt = (hdl_time_counter_t *)flash->dependencies[1];
        flash_var->burn_time = hdl_time_counter_get(time_cnt);
        break;
      }
      flash_var->state = NVM_STATE_COMPLETE;
      break;
    }

    case NVM_STATE_AWAIT_BURNING: {
      hdl_time_counter_t *time_cnt = (hdl_time_counter_t *)flash->dependencies[1];
      uint32_t now = hdl_time_counter_get(time_cnt);
      if(flash_var->op == FLASH_WRITE) {
        if(!(CL_TIME_ELAPSED(flash_var->burn_time, flash->config->write_time, now))) break;
      }
      else { /* erase */
        if(!(CL_TIME_ELAPSED(flash_var->burn_time, flash->config->sector_erase_time, now))) break;
      }
      flash_var->state = NVM_STATE_COMPLETE;
    }

    /* fall through */
    case NVM_STATE_COMPLETE: {
      if(flash_var->nvm_msg->out_status & HDL_NVM_ERROR) flash_var->op = 0;
      if((flash_var->op == FLASH_READ) || 
        (((flash_var->op == FLASH_WRITE) || (flash_var->op == FLASH_ERASE)) &&
          (flash_var->nvm_msg->synced_size >= flash_var->nvm_msg->size)))
        flash_var->op = 0;

      if(!flash_var->op) {
        hdl_gpio_pin_t *wp_pin = (hdl_gpio_pin_t *)flash->dependencies[2];
        hdl_gpio_set_active(wp_pin);
        flash_var->nvm_msg->out_status |= HDL_NVM_STATE_COMPLETE;
        flash_var->nvm_msg->out_status &= ~HDL_NVM_STATE_BUSY;
        flash_var->nvm_msg = NULL;
        flash_var->state = NVM_STATE_IDLE;
      }
      else {
        flash_var->state = NVM_STATE_WP_DISABLE_MSG;
        flash_var->nvm_msg->synced_size = 0;
      }
    }
    /* fall through */
    default:
      break;
  }
  return cancel;
}

static hdl_module_state_t _hdl_w25x20cl(const void *desc, uint8_t enable) {
  hdl_w25x20cl_t *flash = (hdl_w25x20cl_t *)desc;
  hdl_w25x20cl_var_t *flash_var = (hdl_w25x20cl_var_t *)flash->obj_var;
  if(enable) {
    if((flash->config->sector_size == 0) || (flash->config->page_size == 0)) return HDL_MODULE_FAULT;
    hdl_gpio_pin_t *wp_pin = (hdl_gpio_pin_t *)flash->dependencies[2];
    hdl_gpio_pin_t *hold_pin = (hdl_gpio_pin_t *)flash->dependencies[3];
    hdl_gpio_set_active(wp_pin);
    hdl_gpio_set_inactive(hold_pin);
    flash_var->nvm_msg = NULL;
    flash_var->state = NVM_STATE_IDLE;
    coroutine_add(&flash_var->worker, &_flash_worker, (void*)flash);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&flash_var->worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_w25x20cl_transfer(const void *desc, hdl_nvm_message_t *message, uint8_t op) {
  hdl_w25x20cl_t *flash = (hdl_w25x20cl_t *)desc;
  hdl_w25x20cl_var_t *flash_var = (hdl_w25x20cl_var_t *)flash->obj_var;
  if((message == NULL) || (flash_var->nvm_msg != NULL)) return HDL_FALSE;
  message->out_status = HDL_NVM_STATE_BUSY;
  message->synced_size = 0;
  flash_var->nvm_msg = message;
  flash_var->op = op;
  return HDL_TRUE;
}

static uint8_t _hdl_w25x20cl_info_get(const void *desc, hdl_nvm_info_t *out_info) {
  hdl_w25x20cl_t *flash = (hdl_w25x20cl_t *)desc;
  if(out_info != NULL) {
    out_info->sector_size = flash->config->sector_size;
    out_info->mtu_size = flash->config->page_size;
    out_info->volume = flash->config->size;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _hdl_w25x20cl_read(const void *desc, hdl_nvm_message_t *message) {
  return _hdl_w25x20cl_transfer(desc, message, FLASH_READ);
}

static uint8_t _hdl_w25x20cl_write(const void *desc, hdl_nvm_message_t *message) {
  return _hdl_w25x20cl_transfer(desc, message, FLASH_WRITE);
}

static uint8_t _hdl_w25x20cl_erase(const void *desc, hdl_nvm_message_t *message) {
  return _hdl_w25x20cl_transfer(desc, message, FLASH_ERASE);
}

static uint8_t _hdl_w25x20cl_cancel(const void *desc) {
  hdl_w25x20cl_t *flash = (hdl_w25x20cl_t *)desc;
  hdl_w25x20cl_var_t *flash_var = (hdl_w25x20cl_var_t *)flash->obj_var;
  if(flash_var->nvm_msg == NULL) return HDL_TRUE;
  flash_var->cancel = 1;
  return HDL_FALSE;
}


const hdl_nvm_iface_t hdl_w25x20cl_iface = {
  .init = &_hdl_w25x20cl,
  .cancel = &_hdl_w25x20cl_cancel,
  .read = &_hdl_w25x20cl_read,
  .write = &_hdl_w25x20cl_write,
  .erase = &_hdl_w25x20cl_erase,
  .info = &_hdl_w25x20cl_info_get
};
