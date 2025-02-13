#include "hdl_iface.h"

typedef struct {
  coroutine_t worker;
  hdl_nvm_message_t *message;
  uint8_t wrk_state;
} hdl_mcu_flash_var_t;

#define WRK_STATE_START            0
#define WRK_STATE_READ             1
#define WRK_STATE_VALIDATE         2
#define WRK_STATE_ERASE_PAGE       3
#define WRK_STATE_ERASE_AWAITE     4
#define WRK_STATE_WRITE            5
#define WRK_STATE_WRITE_AWAITE     6
#define WRK_STATE_COMPLETE         7

static uint8_t _flash_worker(coroutine_t *this, uint8_t cancel, void *arg) {
  (void)this;
  hdl_mcu_flash_t *flash = (hdl_mcu_flash_t *)arg;
  hdl_mcu_flash_var_t *flash_var = (hdl_mcu_flash_var_t *)flash->obj_var;
  if(flash_var->message != NULL) {
    hdl_nvm_message_t *message = flash_var->message;
    switch (flash_var->wrk_state) {
      case WRK_STATE_START:
        message->transferred = 0;
        if ((!message->options) ||
            ((message->options & ~HDL_NVM_OPTION_ERASE) && 
              ((message->buffer == NULL) || (message->options & HDL_NVM_OPTION_ERASE))) ||
            ((message->options & HDL_NVM_OPTION_READ) && (message->options & HDL_NVM_OPTION_WRITE)) ||
            (message->size == 0)) {
          message->status |= HDL_NVM_ERROR_BAD_ARG;
          flash_var->wrk_state = WRK_STATE_COMPLETE;
        }
        else {
          uint32_t fend = (flash->config->page_size * flash->config->pages_amount);
          uint32_t edst = message->address + message->size;
          if(edst > fend) {
            message->status |= HDL_NVM_ERROR_OUT_OF_RANGE;
            flash_var->wrk_state = WRK_STATE_COMPLETE;
          }
          else {
            if(message->options & HDL_NVM_OPTION_ERASE) {
              if((message->address % flash->config->page_size) || (message->size % flash->config->page_size)) {
                message->status |= HDL_NVM_ERROR_PAGE_UNALIGNED;
                flash_var->wrk_state = WRK_STATE_COMPLETE;
              }
              else {
                flash_var->wrk_state = WRK_STATE_ERASE_PAGE;
                fmc_unlock();
              }
            }
            else if(message->options & HDL_NVM_OPTION_WRITE) {
              flash_var->wrk_state = WRK_STATE_WRITE;
              fmc_unlock();
            }
            else if(message->options & HDL_NVM_OPTION_READ) flash_var->wrk_state = WRK_STATE_READ;
            else if(message->options & HDL_NVM_OPTION_VALIDATE) flash_var->wrk_state = WRK_STATE_VALIDATE;
          }
        }
        break;

      case WRK_STATE_WRITE: {
        uint32_t address = flash->config->page_address + message->address + message->transferred;
        uint8_t *src = message->buffer + message->transferred;
        uint8_t *esrc = message->buffer + message->size;
        FMC_CTL |= FMC_CTL_PG;
        FMC_WS |= FMC_WS_PGW;
        while (message->transferred < message->size) {
          uint64_t data = *((uint64_t *)(address & ~0x07UL));
          do {
            if(src != esrc) ((uint8_t *)&data)[address & 0x07] = *src++;
          } while (++address & 0x07);
          REG32(address - 8) = (uint32_t)data;
          REG32(address - 4) = (uint32_t)(data >> 32);
          message->transferred = src - message->buffer;
          if(!(address & 0x07)) break;
        }
        flash_var->wrk_state = WRK_STATE_WRITE_AWAITE;
        break;
      }
      
      case WRK_STATE_WRITE_AWAITE:
        if(!(FMC_STAT & FMC_STAT_BUSY)) {
          FMC_CTL &= ~FMC_CTL_PG;
          FMC_WS &= ~(FMC_WS_PGW);
          flash_var->wrk_state = WRK_STATE_COMPLETE;
          if((FMC_STAT & FMC_STAT_WPERR) || (FMC_STAT & FMC_STAT_PGERR) || (FMC_STAT & FMC_STAT_PGAERR))
            message->status |= HDL_NVM_ERROR_INTERNAL_FAULT;
          else {
            if(message->transferred < message->size)
              flash_var->wrk_state = WRK_STATE_WRITE;
            else if(message->options & HDL_NVM_OPTION_VALIDATE) 
              flash_var->wrk_state = WRK_STATE_VALIDATE;
          }
        }
        break;

      case WRK_STATE_ERASE_PAGE: {
        uint32_t eaddr = flash->config->page_address + message->address + message->transferred;
        /* start page erase */
        FMC_CTL |= FMC_CTL_PER;
        FMC_ADDR = eaddr;
        FMC_CTL |= FMC_CTL_START;
        flash_var->wrk_state = WRK_STATE_ERASE_AWAITE;
        break;
      }
      
      case WRK_STATE_ERASE_AWAITE:
        if(!(FMC_STAT & FMC_STAT_BUSY)) {
          FMC_CTL &= ~FMC_CTL_PER;
          if((FMC_STAT & FMC_STAT_WPERR) || (FMC_STAT & FMC_STAT_PGERR) || (FMC_STAT & FMC_STAT_PGAERR)) {
            message->status |= HDL_NVM_ERROR_INTERNAL_FAULT;
            flash_var->wrk_state = WRK_STATE_COMPLETE;
          }
          else {
            message->transferred += flash->config->page_size;
            if(message->transferred >= message->size) flash_var->wrk_state = WRK_STATE_COMPLETE;
            else flash_var->wrk_state = WRK_STATE_ERASE_PAGE;
          }
        }
        break;

      case WRK_STATE_VALIDATE: {
        uint8_t *src = message->buffer;
        uint8_t *dst = (uint8_t *)(flash->config->page_address + message->address);
        uint32_t i = 0;
        for(; i < message->size; i++) {
          if(src[i] != dst[i]) {
            message->status |= HDL_NVM_ERROR_VALIDATION_FAULT;
            break;
          }
        }
        message->transferred = i;
        flash_var->wrk_state = WRK_STATE_COMPLETE;
        break;
      }

      case WRK_STATE_READ:
        mem_cpy(message->buffer, (uint8_t *)(flash->config->page_address + message->address), message->size);
        message->transferred = message->size;
        flash_var->wrk_state = WRK_STATE_COMPLETE;
        break;

      case WRK_STATE_COMPLETE:
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR | FMC_FLAG_PGAERR);
        fmc_lock();
        HDL_REG_MODIFY(message->status, HDL_NVM_STATE, HDL_NVM_STATE_COMPLETE);
        flash_var->message = NULL;
      default:
        break;
    }    
  }
  return cancel;
}

static hdl_module_state_t _hdl_nvm_init(const void *desc, uint8_t enable) {
  hdl_mcu_flash_t *flash = (hdl_mcu_flash_t *)desc;
  hdl_mcu_flash_var_t *flash_var = (hdl_mcu_flash_var_t *)flash->obj_var;
  if(enable) {
    coroutine_add(&flash_var->worker, &_flash_worker, (void *)flash);
    return HDL_MODULE_ACTIVE;
  }
  coroutine_cancel(&flash_var->worker);
  return HDL_MODULE_UNLOADED;
}

static uint8_t _hdl_nvm_info_get(const void *desc, hdl_nvm_info_t *out_info) {
  hdl_mcu_flash_t *flash = (hdl_mcu_flash_t *)desc;
  if(out_info != NULL) {
    out_info->page_size = flash->config->page_size;
    out_info->volume = flash->config->page_size * flash->config->pages_amount;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

static uint8_t _hdl_nvm_transfer(const void *desc, hdl_nvm_message_t *message) {
  hdl_mcu_flash_t *flash = (hdl_mcu_flash_t *)desc;
  hdl_mcu_flash_var_t *flash_var = (hdl_mcu_flash_var_t *)flash->obj_var;
  if((hdl_state(flash) != HDL_MODULE_UNLOADED) && (flash_var->message == NULL)) {
    message->status = HDL_NVM_STATE_BUSY;
    flash_var->message = message;
    flash_var->wrk_state = WRK_STATE_START;
    return HDL_TRUE;
  }
  return HDL_FALSE;
}

const hdl_nvm_iface_t mcu_flash_iface = {
  .init = &_hdl_nvm_init,
  .info = &_hdl_nvm_info_get,
  .transfer = &_hdl_nvm_transfer
};
